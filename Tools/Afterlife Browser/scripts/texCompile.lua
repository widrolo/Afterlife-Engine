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

local cmd = string.format('nvcompress -color -bc1 "%s" "%s" > /dev/null 2>&1', input_path, output_path)
local ok = os.execute(cmd)

if not ok then
    io.stderr:write("Compression failed\n")
    os.exit(1)
end

-- nvcompress encodes many blocks in BC1 3-color mode (a.k.a. DXT1 with 1-bit
-- alpha), where texel index 2 is transparent black.  In-game those blocks show
-- up as black "holes".  Rewrite every 3-color block as an opaque 4-color block.
local function makeOpaqueBC1(path)
    local f = io.open(path, "rb")
    if not f then return end
    local data = f:read("*a")
    f:close()

    local function u32(o) return data:byte(o+1) + data:byte(o+2)*256 + data:byte(o+3)*65536 + data:byte(o+4)*16777216 end
    local function u16(o) return data:byte(o+1) + data:byte(o+2)*256 end

    if data:sub(1,4) ~= "DDS " or data:sub(85,88) ~= "DXT1" then return end

    local flags    = u32(8)
    local width    = u32(16)
    local height   = u32(12)
    local mipcount = u32(28)
    if mipcount == 0 or flags & 0x20000 == 0 then mipcount = 1 end

    local chunks = { data:sub(1, 128) }
    local cur  = 129
    local fixed = 0
    local w, h = width, height

    for level = 1, mipcount do
        local nx = math.max(1, math.ceil(w/4))
        local ny = math.max(1, math.ceil(h/4))
        for i = 0, nx*ny - 1 do
            local off = cur + i*8
            local c0 = u16(off-1)
            local c1 = u16(off+1)
            if c0 > c1 then
                chunks[#chunks+1] = data:sub(off, off+7)
            else
                local bits = u32(off+3)
                local nb   = 0
                local nc0, nc1 = c0, c1
                if c0 == c1 then
                    -- flat-color block: keep the color, aim every texel at it,
                    -- give it a strictly-lower (unused) second endpoint.
                    if c0 == 0 then nc0, nc1 = 1, 0 else nc1 = c0 - 1 end
                else
                    -- ordered 3-color block: swap endpoints so c0 > c1, then
                    -- remap indices: 1->0 (light), 0/2/3->1 (dark).
                    nc0, nc1 = c1, c0
                    for px = 0, 15 do
                        local idx = (bits >> (px*2)) & 3
                        if idx == 1 then idx = 0 else idx = 1 end
                        nb = nb | (idx << (px*2))
                    end
                end
                local b1 = nb & 0xff
                local b2 = (nb >> 8) & 0xff
                local b3 = (nb >> 16) & 0xff
                local b4 = (nb >> 24) & 0xff
                local e1 = nc0 % 256
                local e2 = math.floor(nc0/256) % 256
                local e3 = nc1 % 256
                local e4 = math.floor(nc1/256) % 256
                chunks[#chunks+1] = string.char(e1, e2, e3, e4, b1, b2, b3, b4)
                fixed = fixed + 1
            end
        end
        cur = cur + nx*ny*8
        w = math.max(1, math.floor(w/2))
        h = math.max(1, math.floor(h/2))
    end

    if fixed > 0 then
        local fo = io.open(path, "wb")
        fo:write(table.concat(chunks))
        fo:close()
    end
end

makeOpaqueBC1(output_path)

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