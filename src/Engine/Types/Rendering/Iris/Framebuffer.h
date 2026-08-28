#pragma once
#include <string>

#include "Handles.h"
#include "Engine/WTL/vector.h"

namespace Iris
{
    struct FramebufferDesc
    {
        uint32 width;
        uint32 height;
        uint32 layers = 1;
        bool hasDepth;
        std::string debugName;
        ResourceTableLayoutHandle resourceTableLayout;
        // Used for the framebuffer's descriptor set when the layout's first image binding
        // is a combined image sampler.
        SamplerHandle sampler;
    };

    enum class FramebufferBindKind
    {
        Color,
        Depth
    };
}
