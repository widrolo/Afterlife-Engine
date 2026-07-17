#pragma once
#include <string>
#include <Engine/Types/CommonTypes.h>

#include "DDS.h"

namespace WEngine
{
    struct TextureInfo
    {
        byte* data;
        int32 width;
        int32 height;
        int32 channels;
    };
    struct TextureInfoDDS
    {
        byte* data;
        uint32_t width;
        uint32_t height;
        uint32_t mipCount;
        BC format;
    };
}
