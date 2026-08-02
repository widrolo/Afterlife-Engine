#if GPU_BACKEND == GPU_VULKAN

#include <Engine/Core/System/Iris.h>

#include "Helpers/Helpers.h"

namespace Iris
{
    CommandBufferHandle BeginCommandBuffer(QueueType queue)
    {
        PrintNotImplemented("BeginCommandBuffer");
        return 0;
    }

    void EndCommandBuffer(CommandBufferHandle cmd)
    {
        PrintNotImplemented("EndCommandBuffer");
    }

    void SubmitCommandBuffer(CommandBufferHandle cmd)
    {
        PrintNotImplemented("SubmitCommandBuffer");
    }

    void SubmitCommandBuffers(const CommandBufferHandle* cmds, sizeT count)
    {
        PrintNotImplemented("SubmitCommandBuffers");
    }

    void BeginRenderPass(CommandBufferHandle cmd, const RenderPassBeginDesc& desc)
    {
        PrintNotImplemented("BeginRenderPass");
    }

    void EndRenderPass(CommandBufferHandle cmd)
    {
        PrintNotImplemented("EndRenderPass");
    }

    void BeginComputePass(CommandBufferHandle cmd)
    {
        PrintNotImplemented("BeginComputePass");
    }

    void EndComputePass(CommandBufferHandle cmd)
    {
        PrintNotImplemented("EndComputePass");
    }

    void BeginCopyPass(CommandBufferHandle cmd)
    {
        PrintNotImplemented("BeginCopyPass");
    }

    void EndCopyPass(CommandBufferHandle cmd)
    {
        PrintNotImplemented("EndCopyPass");
    }
}

#endif