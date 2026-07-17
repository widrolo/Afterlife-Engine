#include "DDS.h"

DDSFile LoadDDS(const std::string& path)
{
    DDSFile r;
    std::ifstream f(path, std::ios::binary);
    if (!f) return r;

    uint32_t magic; f.read((char*)&magic, 4);
    if (magic != 0x20534444) return r; // "DDS "

    uint32_t hdr[31]; f.read((char*)hdr, sizeof(hdr)); // skip to pixel format
    uint32_t& pf_flags   = hdr[19];
    uint32_t& pf_fourcc  = hdr[20];

    r.h     = hdr[2];
    r.w     = hdr[3];
    r.mips  = hdr[6] ? hdr[6] : 1;

    // FourCC codes
    if (pf_fourcc == 0x31545844)        r.fmt = BC::BC1;  // "DXT1"
    else if (pf_fourcc == 0x31495441)   r.fmt = BC::BC4;  // "ATI1"
    else if (pf_fourcc == 0x32495441)   r.fmt = BC::BC5;  // "ATI2"
    else if (pf_fourcc == 0x30315844)                    // "DX10" — fall through
    {
        uint32_t dx10[5]; f.read((char*)dx10, sizeof(dx10));
        switch (dx10[0]) {
            case 71: r.fmt = BC::BC1; break;
            case 80: r.fmt = BC::BC4; break;
            case 83: r.fmt = BC::BC5; break;
            default: return r;
        }
    }
    else return r;

    // Skip DX10 extension header if present
    if (pf_fourcc == 0x30315844) {     // "DX10"
        uint32_t dx10[5]; f.read((char*)dx10, sizeof(dx10));
        switch (dx10[0]) {
            case 71: r.fmt = BC::BC1; break;
            case 80: r.fmt = BC::BC4; break;
            case 83: r.fmt = BC::BC5; break;
            default: return r;
        }
    }

    auto block_size = [](BC c) -> uint32_t {
        return (c == BC::BC5) ? 16u : 8u;
    };

    uint32_t total = 0, mw = r.w, mh = r.h;
    for (uint32_t i = 0; i < r.mips; ++i) {
        total += ((mw + 3) / 4) * ((mh + 3) / 4) * block_size(r.fmt);
        mw = std::max(1u, mw / 2); mh = std::max(1u, mh / 2);
    }

    r.data.resize(total);
    f.read((char*)r.data.data(), total);
    return r;
}