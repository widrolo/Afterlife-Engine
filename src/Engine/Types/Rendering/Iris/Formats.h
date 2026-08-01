#pragma once
#include "Engine/Types/CommonTypes.h"

namespace Iris
{
    enum class ImgFormat : uint8
    {
        Unknown,

        // -------- Render Targets Only --------
        RGBA8_UNorm,
        BGRA8_UNorm,
        RGBA8_SRGB,
        BGRA8_SRGB,

        // ------------ Depth Only -------------
        D24_UNorm_S8_UInt,

        // ----------- Texture Only ------------
        BC1,    // Albedo
        BC4,    // Heightmaps and single channel stuff
        BC5,    // Dual channel normal maps

        ImgFormat_Count
    };

    // Its obvious I dislike the idea that Khronos had about merging image formats and vertex formats
    enum class VertFormat : uint8
    {
        Float1,
        Float2,
        Float3,
        Float4,
        VertFormat_Count
    };
}
