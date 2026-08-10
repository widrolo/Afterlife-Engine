#pragma once
#include <array>

#include "FixedFunction.h"
#include "Handles.h"
#include "Engine/Math/Vector.h"
#include "Engine/Types/Rendering/Color.h"


namespace Iris
{
    struct AttachmentOps
    {
        LoadOp  loadOp = LoadOp::Clear;
        StoreOp storeOp = StoreOp::Store;
        LoadOp  stencilLoadOp = LoadOp::DontCare;
        StoreOp stencilStoreOp = StoreOp::DontCare;
        WEngine::Color clearColor = WEngine::Color::Black;
        float32 clearDepth = 1.0f;
        uint8 clearStencil = 0;
    };

    struct RenderPassBeginDesc
    {
        FramebufferHandle framebuffer;
        AttachmentOps colorAttachment;
        AttachmentOps depthStencil;
        bool hasDepthStencil = false;
    };

    struct Viewport
    {
        WEngine::Vector2 pos;
        WEngine::Vector2 extent;
        float32 minDepth = 0.0f;
        float32 maxDepth = 0.0f;
    };
}
