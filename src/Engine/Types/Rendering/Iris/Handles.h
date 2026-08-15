#pragma once

#include "Engine/Types/CommonTypes.h"
#include "Engine/Util/Handles.h"

namespace Iris
{
    DEFINE_OPAQUE_HANDLE(BufferHandle);
    DEFINE_OPAQUE_HANDLE(TextureHandle);
    DEFINE_OPAQUE_HANDLE(SamplerHandle);
    DEFINE_OPAQUE_HANDLE(ShaderHandle);
    DEFINE_OPAQUE_HANDLE(ResourceTableLayoutHandle)
    DEFINE_OPAQUE_HANDLE(ResourceTableHandle)
    DEFINE_OPAQUE_HANDLE(GraphicsPipelineHandle);
    DEFINE_OPAQUE_HANDLE(ComputePipelineHandle);
    DEFINE_OPAQUE_HANDLE(FramebufferHandle);
    DEFINE_OPAQUE_HANDLE(CommandBufferHandle);
    DEFINE_OPAQUE_HANDLE(CopyBufferHandle);
}
