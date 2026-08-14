local input_path  = projPath .. assetPath
local stem        = assetPath:gsub("%.[^.]+$", "")   -- "Images/Missing"
local output_path = outPath .. stem .. ".dds"        -- ".../TestOut/Images/Missing.dds"

local out_dir = output_path:match("^(.*)/[^/]*$")
if out_dir then
    os.execute('mkdir -p "' .. out_dir .. '"')
end

local probe = "/tmp/nvcompress_path.txt"
os.execute('command -v nvcompress > "' .. probe .. '" 2>/dev/null')
local nv_path = ""
local f = io.open(probe, "r")
if f then
    nv_path = f:read("*a")
    f:close()
    os.remove(probe)
end
if nv_path == "" then
    io.stderr:write("nvcompress not found on your PATH.\n")
    io.stderr:write("Install with:  sudo apt install nvidia-texture-tools\n")
    os.exit(1)
end

local cmd = string.format('nvcompress -bc1 "%s" "%s" > /dev/null 2>&1', input_path, output_path)
local ok = os.execute(cmd)

if ok then
    local f = io.open(output_path, "rb")
    local size_kb = 0
    if f then
        size_kb = math.ceil(f:seek("end") / 1024)
        f:close()
    end
    if size_kb > 0 then
        print(string.format("Done — BC1 · RGB, 0.5 bpp — %d KB — %s.dds", size_kb, stem))
    else
        io.stderr:write("Compression failed: no output file\n")
        os.exit(1)
    end
else
    io.stderr:write("Compression failed\n")
    os.exit(1)
end
