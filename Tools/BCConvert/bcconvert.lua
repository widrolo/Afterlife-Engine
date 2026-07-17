-- bcconvert.lua — Convert PNGs to BCn DDS textures (Linux / nvcompress)
-- Usage: lua bcconvert.lua <file.png> <-color|-normal|-height>

-- -----------------------------------------------------------
-- 1. Argument parsing
-- -----------------------------------------------------------
if #arg < 2 then
    print([[
Usage: lua bcconvert.lua <input.png> <flag>

Flags:
  -color    RGB color texture   →  BC1  (0.5 bpp, no alpha)
  -normal   Tangent-space normal →  BC5  (1.0 bpp, 2-channel RG)
  -height   Single-channel      →  BC4  (0.5 bpp, grayscale)

Output lands in the same folder with a .dds extension.
]])
    os.exit(1)
end

local input_path = arg[1]
local flag       = arg[2]

-- -----------------------------------------------------------
-- 2. Validate input
-- -----------------------------------------------------------
local function file_exists(path)
    local f = io.open(path, "r")
    if f then f:close(); return true end
    return false
end

if not file_exists(input_path) then
    io.stderr:write("File not found: " .. input_path .. "\n")
    os.exit(1)
end

if flag ~= "-color" and flag ~= "-normal" and flag ~= "-height" then
    io.stderr:write("Unknown flag: " .. flag .. "\n")
    io.stderr:write("Must be one of: -color  -normal  -height\n")
    os.exit(1)
end

-- -----------------------------------------------------------
-- 3. Build output path  (same dir, .dds extension)
-- -----------------------------------------------------------
local stem = input_path:gsub("%.[^.]+$", "")   -- strip extension
local output_path = stem .. ".dds"

if output_path == input_path then
    io.stderr:write("Refusing to overwrite source file\n")
    os.exit(1)
end

-- -----------------------------------------------------------
-- 4. Ensure nvcompress is available
-- -----------------------------------------------------------
-- The Debian-packaged nvcompress does NOT support --version,
-- so use 'which' to detect it instead.
local which = io.popen("which nvcompress 2>/dev/null", "r")
local nv_path = which and which:read("*a") or ""
if which then which:close() end
if nv_path == "" then
    io.stderr:write([[
nvcompress not found on your PATH.
Install NVIDIA Texture Tools:
  sudo apt install nvidia-texture-tools
(or on some distros the binary is in libnvtt-bin —
 run:  dpkg -L libnvtt-bin | grep bin   to locate it)
]])
    os.exit(1)
end
io.stderr:write("Using: " .. nv_path)

-- -----------------------------------------------------------
-- 5. Build and run the command
-- -----------------------------------------------------------
local fmt_map = {
    ["-color"]  = "-bc1",
    ["-normal"] = "-bc5 -normal",
    ["-height"] = "-bc4",
}

local cmd = string.format('nvcompress %s "%s" "%s"',
                          fmt_map[flag], input_path, output_path)

io.stderr:write("Running: " .. cmd .. "\n")
local ok = os.execute(cmd)

-- -----------------------------------------------------------
-- 6. Report
-- -----------------------------------------------------------
if ok and file_exists(output_path) then
    local f = io.open(output_path, "rb")
    local size_kb = 0
    if f then
        size_kb = math.ceil(f:seek("end") / 1024)
        f:close()
    end
    local label = {
        ["-color"]  = "BC1  ·  RGB,     0.5 bpp",
        ["-normal"] = "BC5  ·  2-ch RG, 1.0 bpp",
        ["-height"] = "BC4  ·  gray,    0.5 bpp",
    }
    print(string.format("Done  —  %s  —  %d KB", label[flag], size_kb))
    print("     ", output_path)
else
    io.stderr:write("Compression failed\n")
    os.exit(1)
end
