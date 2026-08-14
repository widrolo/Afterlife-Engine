-- lets see how good LLMs are at doing tools.


--[[==========================================================================
  meshCompile.lua — Afterlife Static Mesh Format (.asmf) compiler
  ============================================================================

  Called by Exporter::CompileSingleMesh() via:
      RunLuaScript("meshCompile.lua", {
          {"projPath",    ...},   -- e.g. ".../bin/TestProj/"
          {"outPath",     ...},   -- e.g. ".../bin/TestOut/"
          {"assetPath",   ...},   -- e.g. "Meshes/Cube.blend"
          {"meshName",    ...},   -- e.g. "Cube_LOD0"  (mesh extracted by name)
          {"blenderPath", ...}    -- path to the Blender executable
      })

  Source scene : projPath .. assetPath  (mesh object taken by name: meshName)
  Output file  : outPath .. <dir>/<base>__<meshName>.asmf
                 e.g. "Meshes/Table.blend" + mesh "Table_LOD0"
                      -> outPath/Meshes/Table__Table_LOD0.asmf

  All extraction happens in this script: it launches Blender headless with an
  embedded Python exporter, which loads the .blend, finds the mesh object by
  name, triangulates it and dumps position/normal/uv per corner into a temp
  binary file. Lua then packs that into the final .asmf and runs the full
  validation sequence from the spec.

  Format (little endian):
    Header : "ASMF" (4 bytes) | vertex count u64 (8) | index count u64 (8)
    Vertex : position xyz f32 (12) | normal xyz f32 (12) | uv xy f32 (8)
    Index  : u32 (4 bytes) per index
============================================================================]]

local projPath, outPath, assetPath, meshName, blenderPath =
    projPath, outPath, assetPath, meshName, blenderPath

-------------------------------------------------------------------------------
-- Constants ------------------------------------------------------------------
-------------------------------------------------------------------------------

local HEADER_IDENTIFIER = "ASMF"
local HEADER_SIZE       = 4 + 8 + 8     -- identifier + 2 x u64 = 20 bytes
local VERTEX_SIZE       = 12 + 12 + 8   -- pos + normal + uv     = 32 bytes
local INDEX_SIZE        = 4             -- u32 indices
local FLOATS_PER_VERTEX = 8             -- x y z nx ny nz u v

-------------------------------------------------------------------------------
-- Little-endian packing / unpacking ------------------------------------------
-- Uses string.pack/unpack (Lua 5.3+) when available, otherwise pure-Lua
-- fallbacks (works on Lua 5.1 / LuaJIT as well).
-------------------------------------------------------------------------------

local function packU32(v)
    if string.pack then
        return string.pack("<I4", v)
    end
    return string.char(
        v % 256,
        math.floor(v / 256) % 256,
        math.floor(v / 65536) % 256,
        math.floor(v / 16777216) % 256
    )
end

local function packU64(v)
    if string.pack then
        return string.pack("<I8", v)
    end
    return packU32(v % 4294967296) .. packU32(math.floor(v / 4294967296))
end

local function packFloat32(f)
    if string.pack then
        return string.pack("<f", f)
    end
    -- Pure-Lua IEEE-754 single precision, little endian (fallback)
    local sign = 0
    if f ~= f then return "\0\0\192\127" end           -- NaN
    if f == math.huge then return "\0\0\128\127" end   -- +inf
    if f == -math.huge then return "\0\0\128\255" end  -- -inf
    if f < 0 then sign = 0x80000000; f = -f end
    if f == 0 then return packU32(sign) end
    local m, e = math.frexp(f)             -- f = m * 2^e, m in [0.5, 1)
    local biased = e + 126
    local mantissa
    if biased > 0 then
        mantissa = math.floor((2 * m - 1) * 0x800000 + 0.5)
        if mantissa >= 0x800000 then mantissa = 0; biased = biased + 1 end
    else
        mantissa = math.floor(f * 2^149 + 0.5)         -- subnormal
        if mantissa >= 0x800000 then mantissa = 0; biased = 1 end
    end
    return packU32(sign + biased * 0x800000 + mantissa)
end

local function unpackU32(b1, b2, b3, b4)
    return b1 + b2 * 256 + b3 * 65536 + b4 * 16777216
end

local function unpackU64(s)
    local lo = unpackU32(s:byte(1), s:byte(2), s:byte(3), s:byte(4))
    local hi = unpackU32(s:byte(5), s:byte(6), s:byte(7), s:byte(8))
    return lo + hi * 4294967296
end

local function unpackFloat32(b1, b2, b3, b4)
    -- Pure-Lua IEEE-754 single precision decode
    local bits = unpackU32(b1, b2, b3, b4)
    local sign = 1.0
    if bits >= 0x80000000 then sign = -1.0; bits = bits - 0x80000000 end
    local exp  = math.floor(bits / 0x800000)
    local mant = bits % 0x800000
    if exp == 0 then
        if mant == 0 then return sign * 0.0 end
        return sign * (mant * 2^-149)
    elseif exp == 255 then
        if mant == 0 then return sign * math.huge end
        return 0.0 / 0.0
    end
    return sign * (1.0 + mant / 0x800000) * 2^(exp - 127)
end

local function unpackFloat32At(data, pos)
    if string.unpack then
        return string.unpack("<f", data, pos)
    end
    return unpackFloat32(data:byte(pos), data:byte(pos + 1), data:byte(pos + 2), data:byte(pos + 3))
end

-------------------------------------------------------------------------------
-- Path helpers ---------------------------------------------------------------
-------------------------------------------------------------------------------

local function normalizePath(p)
    return (p:gsub("\\", "/"))
end

local function joinPath(dir, file)
    local d = normalizePath(dir)
    if d:sub(-1) ~= "/" then d = d .. "/" end
    return d .. normalizePath(file)
end

-- "Meshes/Table.blend" + mesh "Table_LOD0" -> "Meshes/Table__Table_LOD0.asmf"
local function deriveOutputPath(outPath, assetPath, meshName)
    local p = normalizePath(assetPath)
    local dir, file = p:match("^(.*/)([^/]+)$")
    if not dir then dir, file = "", p end

    local base = file
    if file:lower():match("%.blend$") then
        base = file:sub(1, -7)               -- strip ".blend" (6 chars)
    else
        base = file:gsub("%.[^.]*$", "")     -- strip any other extension
    end
    if base == "" then
        error("Invalid asset path, no file name: " .. assetPath)
    end

    -- mesh names are concatenated with "__"; keep them out of the path
    local safeMesh = tostring(meshName):gsub("[/\\]", "_")
    if safeMesh == "" then error("Empty mesh name") end

    return joinPath(outPath, dir .. base .. "__" .. safeMesh .. ".asmf")
end

-- Best effort directory creation; the host may already create these folders.
local function ensureOutputDirectory(outputPath)
    local dir = outputPath:match("^(.*)/[^/]*$")
    if not dir or dir == "" or not os.execute then return end
    pcall(os.execute, 'mkdir -p "' .. dir .. '"')
end

-------------------------------------------------------------------------------
-- Temporary files ------------------------------------------------------------
-------------------------------------------------------------------------------

local tempCounter = 0

local function makeTempPath(suffix)
    if os.tmpname then
        local ok, p = pcall(os.tmpname)
        if ok and p then return p .. suffix end
    end
    tempCounter = tempCounter + 1
    return joinPath(outPath, "__meshcompile_tmp_" .. tempCounter .. suffix)
end

local function cleanupFile(path)
    if path then pcall(os.remove, path) end
end

-------------------------------------------------------------------------------
-- Python exporter (written to a temp file, executed by Blender headless) -----
-------------------------------------------------------------------------------

local PYTHON_SCRIPT = [[
import bpy
import struct
import sys
import os

STATUS_PATH = None

def fail(msg):
    if STATUS_PATH:
        try:
            with open(STATUS_PATH, "w") as f:
                f.write("ERR: " + msg + "\n")
        except Exception:
            pass
    sys.stderr.write("MESH_EXPORT_ERROR: " + msg + "\n")
    sys.exit(1)

def main():
    global STATUS_PATH

    # Blender strips "--" in some versions and keeps it in others; handle both.
    if "--" in sys.argv:
        args = sys.argv[sys.argv.index("--") + 1:]
    else:
        args = sys.argv[1:]
    if len(args) < 4:
        fail("expected 4 arguments after '--'")

    blend_path, mesh_name, out_path, status_path = args[0], args[1], args[2], args[3]
    STATUS_PATH = status_path

    if not os.path.exists(blend_path):
        fail("blend file does not exist: " + blend_path)

    bpy.ops.wm.open_mainfile(filepath=blend_path)

    if mesh_name not in bpy.data.objects:
        fail("mesh object '" + mesh_name + "' not found in " + blend_path)

    obj = bpy.data.objects[mesh_name]
    if obj.type != "MESH":
        fail("object '" + mesh_name + "' is not a mesh (type: " + obj.type + ")")

    # Use the evaluated (modifier-applied) mesh when possible; fall back to raw.
    mesh = None
    obj_eval = None
    try:
        dg = bpy.context.evaluated_depsgraph_get()
        obj_eval = obj.evaluated_get(dg)
        mesh = obj_eval.to_mesh()
    except Exception:
        obj_eval = None
        mesh = obj.data

    try:
        # Per-loop split normals (respects smooth shading / auto smooth).
        has_split = False
        try:
            mesh.calc_normals_split()
            has_split = True
        except Exception:
            has_split = False

        # Triangulate.
        tris = None
        try:
            mesh.calc_loop_triangles()
            tris = mesh.loop_triangles
        except Exception:
            tris = []
            for poly in mesh.polygons:
                start = poly.loop_start
                n = poly.loop_total
                for i in range(1, n - 1):
                    tris.append((start, start + i, start + i + 1))

        loops = mesh.loops
        verts = mesh.vertices
        uv_layer = None
        if mesh.uv_layers.active is not None:
            uv_layer = mesh.uv_layers.active.data

        # Bake the object's world transform into the mesh (static mesh = final
        # positions). Normals get the rotation only, so non-uniform scale does
        # not skew them.
        mat = obj.matrix_world
        rot = mat.to_quaternion().to_matrix()

        out_verts = []
        out_idx = []

        for tri in tris:
            for loop_index in tri.loops:
                loop = loops[loop_index]
                v = verts[loop.vertex_index]

                pos = mat @ v.co

                if has_split:
                    nrm = rot @ loop.normal
                else:
                    nrm = rot @ v.normal
                nrm.normalize()

                if uv_layer is not None:
                    uv = uv_layer[loop_index].uv
                    u, vt = uv[0], uv[1]
                else:
                    u, vt = 0.0, 0.0

                out_verts += [pos[0], pos[1], pos[2], nrm[0], nrm[1], nrm[2], u, vt]
                out_idx.append(len(out_idx))

        if not out_verts:
            fail("mesh '" + mesh_name + "' has no extractable geometry")

        with open(out_path, "wb") as f:
            f.write(b"ASMFEXPORT")
            f.write(struct.pack("<I", len(out_verts) // 8))
            f.write(struct.pack("<I", len(out_idx)))
            for i in range(0, len(out_verts), 8):
                f.write(struct.pack("<8f", *out_verts[i:i + 8]))
            for idx in out_idx:
                f.write(struct.pack("<I", idx))

        with open(status_path, "w") as f:
            f.write("OK\n")
    finally:
        if obj_eval is not None:
            try:
                obj_eval.to_mesh_clear()
            except Exception:
                pass

main()
]]

-------------------------------------------------------------------------------
-- Blender invocation ---------------------------------------------------------
-------------------------------------------------------------------------------

local function writePythonExporter(path)
    local f, err = io.open(path, "wb")
    if not f then
        error("Cannot write temporary Python exporter: " .. path .. " (" .. tostring(err) .. ")")
    end
    local ok, werr = f:write(PYTHON_SCRIPT)
    f:close()
    if not ok then
        error("Failed writing temporary Python exporter: " .. tostring(werr))
    end
end

local function readFileBinary(path)
    local f, err = io.open(path, "rb")
    if not f then return nil, err end
    local data = f:read("*a")
    f:close()
    return data
end

local function readLogTail(logPath, maxLines)
    local log = readFileBinary(logPath) or ""
    local lines = {}
    for line in (log .. "\n"):gmatch("(.-)\n") do
        lines[#lines + 1] = line
    end
    local n = #lines
    if n == 0 then return "" end
    if n > maxLines then
        return table.concat(lines, "\n", n - maxLines + 1, n)
    end
    return table.concat(lines, "\n", 1, n)
end

-- Runs Blender headless. Success is decided by the status file the Python
-- exporter writes, not by the process exit code.
local function runBlenderExport(blenderPath, blendPath, meshName, dataPath, statusPath, logPath, pyPath)
    if not os.execute then
        error("os.execute is not available in this Lua environment")
    end
    local cmd = string.format(
        '"%s" --background --factory-startup -P "%s" -- "%s" "%s" "%s" "%s" > "%s" 2>&1',
        blenderPath, pyPath, blendPath, meshName, dataPath, statusPath, logPath)

    local ok, how, code = os.execute(cmd)
    if type(ok) == "number" then
        return " (exit code " .. ok .. ")"
    elseif ok == nil then
        return " (could not run: " .. tostring(how) .. ")"
    end
    return " (exit " .. tostring(how) .. " code " .. tostring(code) .. ")"
end

-------------------------------------------------------------------------------
-- Mesh extraction ------------------------------------------------------------
-------------------------------------------------------------------------------

local function parseExport(data, path)
    local MAGIC = "ASMFEXPORT"
    if not data or #data < #MAGIC + 8 then
        error("Blender export data missing or truncated: " .. path)
    end
    if data:sub(1, #MAGIC) ~= MAGIC then
        error("Blender export data has invalid header: " .. path)
    end

    local pos = #MAGIC + 1
    local function readU32()
        local v = unpackU32(data:byte(pos), data:byte(pos + 1), data:byte(pos + 2), data:byte(pos + 3))
        pos = pos + 4
        return v
    end

    local vertexCount = readU32()
    local indexCount  = readU32()

    local expected = (pos - 1) + vertexCount * 32 + indexCount * 4
    if #data ~= expected then
        error(string.format("Blender export data size mismatch: got %d bytes, expected %d", #data, expected))
    end

    local vertices = {}
    for i = 1, vertexCount do
        for j = 1, 8 do
            vertices[#vertices + 1] = unpackFloat32At(data, pos)
            pos = pos + 4
        end
    end

    local indices = {}
    for i = 1, indexCount do
        indices[i] = unpackU32(data:byte(pos), data:byte(pos + 1), data:byte(pos + 2), data:byte(pos + 3))
        pos = pos + 4
    end

    return vertices, indices
end

local function extractMesh(blendPath, meshName, blenderPath)
    local pyPath     = makeTempPath(".py")
    local dataPath   = makeTempPath(".bin")
    local statusPath = makeTempPath(".status")
    local logPath    = makeTempPath(".log")

    local exitInfo = ""
    local ok, err = pcall(function()
        writePythonExporter(pyPath)
        exitInfo = runBlenderExport(blenderPath, blendPath, meshName, dataPath, statusPath, logPath, pyPath)
    end)

    if not ok then
        cleanupFile(pyPath); cleanupFile(dataPath); cleanupFile(statusPath); cleanupFile(logPath)
        error(tostring(err))
    end

    local status = readFileBinary(statusPath)
    local data   = readFileBinary(dataPath)

    if not status or status:sub(1, 2) ~= "OK" then
        local msg = "no status file"
        if status and status:sub(1, 4) == "ERR:" then
            msg = status:sub(6):gsub("%s+$", "")
        end
        local tail = readLogTail(logPath, 25)
        cleanupFile(pyPath); cleanupFile(dataPath); cleanupFile(statusPath); cleanupFile(logPath)
        error("Blender export failed" .. exitInfo .. ": " .. msg
            .. "\n--- blender output (tail) ---\n" .. tail)
    end

    if not data then
        cleanupFile(pyPath); cleanupFile(dataPath); cleanupFile(statusPath); cleanupFile(logPath)
        error("Blender reported OK but produced no data file")
    end

    local ok2, vertices, indices = pcall(parseExport, data, dataPath)
    cleanupFile(pyPath); cleanupFile(dataPath); cleanupFile(statusPath); cleanupFile(logPath)
    if not ok2 then error(vertices) end

    return vertices, indices
end

-------------------------------------------------------------------------------
-- Writing --------------------------------------------------------------------
-------------------------------------------------------------------------------

local function writeAsmf(path, v, idx)
    local vertexCount = #v / FLOATS_PER_VERTEX
    local indexCount  = #idx

    local chunks = { HEADER_IDENTIFIER }
    chunks[#chunks + 1] = packU64(vertexCount)
    chunks[#chunks + 1] = packU64(indexCount)

    for i = 1, #v, FLOATS_PER_VERTEX do
        for j = i, i + FLOATS_PER_VERTEX - 1 do
            chunks[#chunks + 1] = packFloat32(v[j])
        end
    end

    for i = 1, #idx do
        chunks[#chunks + 1] = packU32(idx[i])
    end

    local blob = table.concat(chunks)

    local f, err = io.open(path, "wb")
    if not f then
        error("Cannot open output file for writing: " .. path .. " (" .. tostring(err) .. ")")
    end
    local ok, werr = f:write(blob)
    f:close()   -- file MUST be closed before validation
    if not ok then
        error("Failed writing output file: " .. path .. " (" .. tostring(werr) .. ")")
    end
end

-------------------------------------------------------------------------------
-- Validation (exact order from the .asmf spec) -------------------------------
-------------------------------------------------------------------------------

local function validateFile(path, vertexCount, indexCount)
    -- 1. The file exists ------------------------------------------------------
    -- 2. The file is accessible ------------------------------------------------
    local f, err = io.open(path, "rb")
    if not f then
        local e = tostring(err):lower()
        if e:find("no such file") or e:find("not found") or e:find("cannot find") then
            error("Validation [1/9] failed: file does not exist: " .. path)
        end
        error("Validation [2/9] failed: file exists but is not accessible: " .. path
            .. " (" .. tostring(err) .. ")")
    end

    -- 3. Exact byte size: header + vertex region + index region ---------------
    local size = f:seek("end")
    local expected = HEADER_SIZE + vertexCount * VERTEX_SIZE + indexCount * INDEX_SIZE
    if size ~= expected then
        f:close()
        error(string.format("Validation [3/9] failed: file is %d bytes, expected %d bytes", size, expected))
    end

    -- Read the counts back from the header and validate against the file ------
    f:seek("set", 0)
    local header = f:read(HEADER_SIZE)
    local fileVertexCount = unpackU64(header:sub(5, 12))
    local fileIndexCount  = unpackU64(header:sub(13, 20))

    -- 4. Vertex count must not be zero -----------------------------------------
    if fileVertexCount == 0 then
        f:close()
        error("Validation [4/9] failed: vertex count is zero")
    end

    -- 5. Index count must not be zero ------------------------------------------
    if fileIndexCount == 0 then
        f:close()
        error("Validation [5/9] failed: index count is zero")
    end

    -- 6. Index count must be a multiple of 3 -----------------------------------
    if fileIndexCount % 3 ~= 0 then
        f:close()
        error("Validation [6/9] failed: index count " .. fileIndexCount .. " is not a multiple of 3")
    end

    -- 7. No index may be numerically higher than (vertex count - 1) ------------
    f:seek("set", HEADER_SIZE + fileVertexCount * VERTEX_SIZE)
    local ib = f:read(fileIndexCount * INDEX_SIZE)
    f:close()
    if not ib or #ib ~= fileIndexCount * INDEX_SIZE then
        error("Validation [7/9] failed: could not read index data from: " .. path)
    end
    for i = 1, fileIndexCount do
        local o = (i - 1) * 4
        local index = unpackU32(ib:byte(o + 1), ib:byte(o + 2), ib:byte(o + 3), ib:byte(o + 4))
        if index > fileVertexCount - 1 then
            error("Validation [7/9] failed: index " .. index .. " (position " .. (i - 1)
                .. ") exceeds (vertex count - 1) = " .. (fileVertexCount - 1))
        end
    end

    -- 8. Vertex region size / vertex size must not be fractional ---------------
    -- 9. ... and the quotient must equal the vertex count ----------------------
    local vertexBytes = size - HEADER_SIZE - fileIndexCount * INDEX_SIZE
    if vertexBytes % VERTEX_SIZE ~= 0 then
        error("Validation [8/9] failed: vertex byte region " .. vertexBytes
            .. " is not a multiple of vertex size " .. VERTEX_SIZE)
    end
    local implied = vertexBytes / VERTEX_SIZE
    if implied ~= fileVertexCount then
        error("Validation [9/9] failed: vertex region implies " .. implied
            .. " vertices, header says " .. fileVertexCount)
    end
end

-------------------------------------------------------------------------------
-- Main -----------------------------------------------------------------------
-------------------------------------------------------------------------------

local function main(projPath, outPath, assetPath, meshName, blenderPath)
    if type(projPath) ~= "string" or type(outPath) ~= "string" or type(assetPath) ~= "string" then
        print("[meshCompile] ERROR: missing projPath / outPath / assetPath arguments")
        return false
    end
    if type(meshName) ~= "string" or meshName == "" then
        print("[meshCompile] ERROR: missing meshName argument")
        return false
    end
    if type(blenderPath) ~= "string" or blenderPath == "" then
        print("[meshCompile] ERROR: missing blenderPath argument")
        return false
    end

    local blendPath  = joinPath(projPath, assetPath)
    local outputPath = deriveOutputPath(outPath, assetPath, meshName)

    local vertexCount, indexCount = 0, 0
    local ok, err = pcall(function()
        ensureOutputDirectory(outputPath)

        local vertices, indices = extractMesh(blendPath, meshName, blenderPath)

        vertexCount = #vertices / FLOATS_PER_VERTEX
        indexCount  = #indices
        if vertexCount % 1 ~= 0 or vertexCount == 0 then
            error("Extracted vertex count is invalid: " .. #vertices)
        end
        if indexCount == 0 or indexCount % 3 ~= 0 then
            error("Extracted index count is invalid: " .. indexCount)
        end

        writeAsmf(outputPath, vertices, indices)
        validateFile(outputPath, vertexCount, indexCount)
    end)

    if not ok then
        print("[meshCompile] ERROR: " .. tostring(err))
        if outputPath then
            local removed, rerr = os.remove(outputPath)
            if removed then
                print("[meshCompile] Deleted invalid output file: " .. outputPath)
            elseif not tostring(rerr):find("No such file") then
                print("[meshCompile] WARNING: could not delete output file: " .. outputPath)
            end
        end
        return false
    end

    print(string.format("[meshCompile] Compiled '%s' (mesh '%s') -> %s (%d vertices, %d indices)",
        assetPath, meshName, outputPath, vertexCount, indexCount))
    return true
end

return main(projPath, outPath, assetPath, meshName, blenderPath)
