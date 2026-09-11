-- APMF compiler. Globals: projPath, outPath, assetPath, meshName, blenderPath.
-- Output: outPath/PhysicsMeshes/<source stem>__<sanitized object name>.apmf.

local function quote(value)
    return "'" .. value:gsub("'", "'\\''") .. "'"
end

local function join(dir, name)
    return dir:gsub("/+$", "") .. "/" .. name
end

local function argument(name, value)
    assert(type(value) == "string" and value ~= "" and not value:find("\0", 1, true),
        "Missing or invalid " .. name)
    return value
end

local PYTHON_SCRIPT = [[
import bpy
import struct
import sys

blend_path, mesh_name, output_path = sys.argv[sys.argv.index("--") + 1:]
bpy.ops.wm.open_mainfile(filepath=blend_path)
obj = bpy.data.objects.get(mesh_name)
if obj is None or obj.type != "MESH":
    raise RuntimeError("Mesh object {!r} not found in {}".format(mesh_name, blend_path))

evaluated = obj.evaluated_get(bpy.context.evaluated_depsgraph_get())
mesh = evaluated.to_mesh()
try:
    mesh.calc_loop_triangles()
    world = evaluated.matrix_world
    # The (x, z, -y) rotation preserves handedness. Mirrors already flip CCW.
    reverse_winding = world.to_3x3().determinant() > 0.0
    remap = {}
    vertices = []
    indices = []
    for triangle in mesh.loop_triangles:
        corners = []
        for source_index in triangle.vertices:
            if source_index not in remap:
                remap[source_index] = len(vertices)
                p = world @ mesh.vertices[source_index].co
                vertices.append((p.x, p.z, -p.y))
            corners.append(remap[source_index])
        a, b, c = corners
        indices.extend((a, c, b) if reverse_winding else (a, b, c))

    with open(output_path, "wb") as output:
        output.write(struct.pack("<4sIII", b"APMF", 0, len(vertices), len(indices)))
        for vertex in vertices:
            output.write(struct.pack("<3f", *vertex))
        for index in indices:
            output.write(struct.pack("<I", index))
    print("[physicsMeshCompile] Exported {} of {} evaluated vertices, {} triangles".format(
        len(vertices), len(mesh.vertices), len(indices) // 3))
finally:
    evaluated.to_mesh_clear()
]]

local function validate(path)
    assert(os.execute("test -e " .. quote(path)),
        "Validation [1/10]: output file does not exist: " .. path)

    local vertexCount, indexCount, expected
    do
        local file, err = io.open(path, "rb")
        assert(file, "Validation [2/10]: output file is inaccessible: " .. tostring(err))
        local closedFile <close> = file
        local size = assert(file:seek("end"), "Validation [3/10]: cannot query file size")
        assert(file:seek("set", 0))
        local header = file:read(16)
        assert(header and #header == 16, "Validation [3/10]: truncated header")
        -- Read counts only to calculate size; header validation follows the size check.
        vertexCount, indexCount = string.unpack("<I4I4", header, 9)
        expected = 16 + vertexCount * 12 + indexCount * 4
        assert(size == expected, string.format(
            "Validation [3/10]: file size is %d, expected %d bytes", size, expected))
    end

    local file, err = io.open(path, "rb")
    assert(file, "Cannot reopen output for validation: " .. tostring(err))
    local closedFile <close> = file
    local data = assert(file:read("a"), "Cannot read output for validation")
    assert(#data == expected, "Output size changed during validation")
    assert(data:sub(1, 4) == "APMF", "Validation [4/10]: invalid APMF identifier")
    local readVertices, readIndices = string.unpack("<I4I4", data, 9)
    assert(readVertices == vertexCount and readIndices == indexCount,
        "Output counts changed during validation")
    assert(vertexCount >= 3, "Validation [5/10]: vertex count is less than 3")
    assert(indexCount ~= 0, "Validation [6/10]: index count is zero")
    assert(indexCount % 3 == 0, "Validation [7/10]: index count is not a multiple of 3")

    local indexStart = 17 + vertexCount * 12
    local referenced = {}
    for pos = indexStart, #data, 4 do
        local index = string.unpack("<I4", data, pos)
        referenced[index] = true
    end
    for index = 0, vertexCount - 1 do
        assert(referenced[index], "Validation [8/10]: unreferenced vertex " .. index)
    end
    for pos = 17, indexStart - 1, 4 do
        local value = string.unpack("<f", data, pos)
        assert(value == value and value ~= math.huge and value ~= -math.huge,
            "Validation [9/10]: non-finite position at byte offset " .. (pos - 1))
    end
    for pos = indexStart, #data, 4 do
        local index = string.unpack("<I4", data, pos)
        assert(index < vertexCount, "Validation [10/10]: out-of-range index " .. index)
    end
    return vertexCount, indexCount
end

local outputPath, pythonPath
local ok, err = pcall(function()
    local outputDir = join(argument("outPath", outPath), "PhysicsMeshes")
    local asset = argument("assetPath", assetPath):gsub("\\", "/")
    local basename = assert(asset:match("([^/]+)$"), "Asset path has no filename")
    local stem = basename:gsub("%.[^.]*$", "")
    assert(stem ~= "", "Asset path has no filename stem")
    local safeName = argument("meshName", meshName):gsub("[/\\]", "_")
    outputPath = join(outputDir, stem .. "__" .. safeName .. ".apmf")

    -- Never let a failed Blender invocation validate an old successful export.
    local removed, removeError, code = os.remove(outputPath)
    assert(removed or code == 2, "Cannot delete stale output: " .. tostring(removeError))
    local source = join(argument("projPath", projPath), asset)
    argument("blenderPath", blenderPath)
    assert(os.execute("mkdir -p -- " .. quote(outputDir)), "Cannot create output directory: " .. outputDir)

    pythonPath = os.tmpname()
    do
        local file = assert(io.open(pythonPath, "wb"))
        local closedFile <close> = file
        assert(file:write(PYTHON_SCRIPT))
        assert(file:flush())
    end
    local success, how, status = os.execute(quote(blenderPath)
        .. " --background --factory-startup --disable-autoexec --python-exit-code 1 --python "
        .. quote(pythonPath) .. " -- " .. quote(source) .. " " .. quote(meshName) .. " " .. quote(outputPath))
    assert(success and how == "exit" and status == 0,
        "Blender export failed (" .. tostring(how) .. " " .. tostring(status) .. "); see Blender output above")

    local vertexCount, indexCount = validate(outputPath)
    print(string.format("[physicsMeshCompile] Compiled '%s' (mesh '%s') -> %s (%d vertices, %d indices, %d triangles)",
        assetPath, meshName, outputPath, vertexCount, indexCount, indexCount // 3))
end)

if pythonPath then
    local removed, cleanupError = os.remove(pythonPath)
    if not removed then
        ok, err = false, tostring(err or "") .. " Cannot remove temporary exporter: " .. tostring(cleanupError)
    end
end
if not ok then
    if outputPath then
        local removed, cleanupError, code = os.remove(outputPath)
        if not removed and code ~= 2 then
            err = tostring(err) .. " Cannot delete invalid output: " .. tostring(cleanupError)
        end
    end
    local message = "[physicsMeshCompile] ERROR: " .. tostring(err)
    print(message)
    error(message, 0)
end
return true
