#pragma once
#include "Engine/Util/BitwiseMacros.h"

namespace Iris
{
    enum class BufferUsage : uint8
    {
        None            = 0,
        Vertex          = BIT(0),
        Index           = BIT(1),
        Uniform         = BIT(2),
        Storage         = BIT(3),
        TransferSrc     = BIT(4),
        TransferDst     = BIT(5),
    };
    DEFINE_ENUM_BITWISE(BufferUsage)

    enum class TextureUsage : uint8
    {
        None           = 0,
        Sampled        = BIT(0),
        Storage        = BIT(1),
        ColorTarget    = BIT(2),
        DepthTarget    = BIT(3),
        TransferSrc    = BIT(4),
        TransferDst    = BIT(5),
    };
    DEFINE_ENUM_BITWISE(TextureUsage)

    // In theory, there are also 3D textures. However, im not keep on experimenting with it just yet.
    // But I am keeping this an Enum instead of a using for the future.
    enum class TextureType : uint8
    {
        Texture2D,
    };

    enum class SampleCount : uint8
    {
        Samples1        = 1,
        Samples2        = 2,
        Samples4        = 4,
        Samples8        = 8,
    };

    // Yes, mesh shaders exist; no im never ever using them ever.
    enum class ShaderStage : uint8
    {
        Vertex          = BIT(0),
        Fragment        = BIT(1),
        Compute         = BIT(2),
        Geometry        = BIT(3),
        TessControl     = BIT(4),
        TessEval        = BIT(5),
    };
    DEFINE_ENUM_BITWISE(ShaderStage)
}
