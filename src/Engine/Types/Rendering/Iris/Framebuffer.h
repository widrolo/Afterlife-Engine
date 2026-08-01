#pragma once
#include <string>

#include "Handles.h"
#include "Engine/WTL/vector.h"

namespace Iris
{
    struct FramebufferDesc
    {
        wtl::vector<TextureHandle> colorAttachments;
        TextureHandle depthStencilAttachment;
        uint32 width;
        uint32 height;
        uint32 layers = 1;
        std::string debugName;
    };
}
