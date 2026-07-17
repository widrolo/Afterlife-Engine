// Thank you Deepseek

#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>

enum class BC { BC1, BC4, BC5 };
struct DDSFile {
    std::vector<uint8_t> data;
    uint32_t w = 0, h = 0, mips = 0;
    BC fmt = BC::BC1;
};

DDSFile LoadDDS(const std::string& path);
