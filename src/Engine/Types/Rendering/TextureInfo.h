#pragma once
#include <string>
#include <Engine/Types/CommonTypes.h>

namespace WEngine
{
    struct TextureInfo
    {
        byte* data;
        int32 width;
        int32 height;
        int32 channels;
    };
}
