// Thank you Deepseek

#pragma once

#include <string>
#include "Engine/WTL/vector.h"

enum class BC
{
    BC1,
    BC4,
    BC5
};
struct DDSFile
{
    wtl::vector<byte> data;
    uint32 width;
    uint32 height;
    uint32 mips;
    BC format;
};

DDSFile LoadDDS(const std::string& path);
