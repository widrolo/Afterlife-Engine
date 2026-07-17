#include "DDS.h"

#include <fstream>

struct DDSPixelFormat
{
    uint32 dwSize;
    uint32 dwFlags;
    uint32 dwFourCC;
    uint32 dwRGBBitCount;
    uint32 dwRBitMask;
    uint32 dwGBitMask;
    uint32 dwBBitMask;
    uint32 dwABitMask;
};

struct DDSHeader
{
    uint32 dwSize;
    uint32 dwFlags;
    uint32 dwHeight;
    uint32 dwWidth;
    uint32 dwPitchOrLinearSize;
    uint32 dwDepth;
    uint32 dwMipMapCount;
    uint32 dwReserved1[11];
    DDSPixelFormat ddspf;
    uint32 dwCaps;
    uint32 dwCaps2;
    uint32 dwCaps3;
    uint32 dwCaps4;
    uint32 dwReserved2;
};

struct DDSHeaderDXT10
{
    uint32 dxgiFormat;
    uint32 resourceDimension;
    uint32 miscFlag;
    uint32 arraySize;
    uint32 miscFlags2;
};

enum class BCCode : dword
{
    BC1 = 0x31545844,
    BC4 = 0x31495441,
    BC5 = 0x32495441,
    DX10 = 0x30315844,
};

enum class DXGIFormat : dword
{
    BC1= 71,
    BC4= 80,
    BC5= 83,
};

// "DDS " but backwards because endian
constexpr dword signature = 0x20534444;

sizeT BlockSize(BC bc)
{
    if (bc == BC::BC5)
        return 16;
    return 8;
}

DDSFile LoadDDS(const std::string& path)
{
    DDSHeader header{};
    DDSHeaderDXT10 dx10{};
    DDSFile dds{};

    std::ifstream file(path, std::ios::binary);
    if (!file)
        return dds;

    uint32 sig;
    file.read((char*)&sig, 4);

    if (sig != signature)
        return dds;

    file.read((char*)&header, sizeof(DDSHeader));
    uint32& pixelFlags = header.ddspf.dwFlags;
    uint32& pixelFourCC = header.ddspf.dwFourCC;

    dds.height = header.dwHeight;
    dds.width = header.dwWidth;
    dds.mips = header.dwMipMapCount ? header.dwMipMapCount : 1;

    // FourCC codes
    if (pixelFourCC == (dword)BCCode::BC1)
        dds.format = BC::BC1;
    else if (pixelFourCC == (dword)BCCode::BC4)
        dds.format = BC::BC4;
    else if (pixelFourCC == (dword)BCCode::BC5)
        dds.format = BC::BC5;
    else if (pixelFourCC == (dword)BCCode::DX10)
    {
        file.read((char*)&dx10, sizeof(DDSHeaderDXT10));
        switch (dx10.dxgiFormat)
        {
            case (dword)DXGIFormat::BC1: dds.format = BC::BC1; break;
            case (dword)DXGIFormat::BC4: dds.format = BC::BC4; break;
            case (dword)DXGIFormat::BC5: dds.format = BC::BC5; break;
            default: return dds;
        }
    }
    else
        return dds;

    uint32 total = 0;
    uint32 mipWidth = dds.width;
    uint32 mipHeight = dds.height;
    for (sizeT i = 0; i < dds.mips; ++i)
    {
        uint32 blocksWide = (mipWidth + 3) / 4;
        uint32 blocksHigh = (mipHeight + 3) / 4;

        total += blocksWide * blocksHigh * BlockSize(dds.format);

        mipWidth = std::max(1u, mipWidth / 2);
        mipHeight = std::max(1u, mipHeight / 2);
    }

    dds.data.resize(total);
    file.read((char*)dds.data.data(), total);
    return dds;
}