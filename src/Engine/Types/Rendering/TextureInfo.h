#pragma once
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
        uint32 width;
        uint32 height;
        uint32 mipCount;
        BC format;
    };
}
