#pragma once

#include "Engine/Types/CommonTypes.h"

namespace Iris
{
    enum class TopologyType : uint8
    {
        Point_List,
        Line_List,
        Triangle_List,

        Line_Strip,
        Triangle_Strip,

        // Really only useful for 2D, but Ui exists so we might as well.
        Triangle_Fan,
    };

    using IndexFormat = uint32;

    enum class CompareOp : uint8
    {
        Never, Less, Equal, LessEqual, Greater, NotEqual, GreaterEqual, Always
    };
    enum class CullMode : uint8 { None, Front, Back };
    enum class FrontFace : uint8 { CounterClockwise, Clockwise };
    enum class FillMode : uint8 { Solid, Wireframe };
    enum class BlendFactor : uint8
    {
        Zero, One, SrcColor, OneMinusSrcColor, DstColor, OneMinusDstColor, SrcAlpha, OneMinusSrcAlpha, DstAlpha,
        OneMinusDstAlpha, ConstantColor, OneMinusConstantColor, ConstantAlpha, OneMinusConstantAlpha, SrcAlphaSaturate,
        Src1Color, OneMinusSrc1Color, Src1Alpha, OneMinusSrc1Alpha
    };
    enum class BlendOp : uint8 { Add, Subtract, ReverseSubtract, Min, Max };
    enum class StencilOp : uint8 { Keep, Zero, Replace, IncrementClamp, DecrementClamp, IncrementWrap, DecrementWrap, Invert };
    enum class LogicOp : uint8
    {
        Clear, Set, Copy, CopyInverted, Noop, Inverted, And, Nand, Or, Nor, Xor, Equiv, AndReverse, AndInverted,
        OrReverse, OrInverted
    };
    enum class FilterMode : uint8 { Nearest, Linear };
    enum class MipFilterMode : uint8 { None, Nearest, Linear };
    enum class SamplerAddressMode : uint8 { Repeat, MirrorRepeat, ClampToEdge, ClampToBorder };
    enum class BorderColor : uint8 { TransparentBlack, OpaqueBlack, OpaqueWhite };
    enum class LoadOp : uint8 { Load, Clear, DontCare };
    enum class StoreOp : uint8 { Store, DontCare };
    enum class QueueType : uint8 { Graphics, Compute, Copy };
}
