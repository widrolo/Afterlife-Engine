#if GPU_BACKEND == GPU_VULKAN

#include <Engine/Core/System/Iris.h>

#include "Helpers/Helpers.h"

namespace Iris
{
    void BindGraphicsPipeline(CommandBufferHandle cmd, GraphicsPipelineHandle pipeline)
    {
        PrintNotImplemented("BindGraphicsPipeline");
    }

    void BindComputePipeline(CommandBufferHandle cmd, ComputePipelineHandle  pipeline)
    {
        PrintNotImplemented("BindComputePipeline");
    }

    void BindResourceTable(CommandBufferHandle cmd, uint32 slot, ResourceTableHandle table)
    {
        PrintNotImplemented("BindResourceTable");
    }

    void SetPushConstants(CommandBufferHandle cmd, const byte* data, sizeT size)
    {
        PrintNotImplemented("SetPushConstants");
    }

    void BindVertexBuffers(CommandBufferHandle cmd, uint32 firstBinding, const BufferHandle* buffers,
        const sizeT* offsets, sizeT count)
    {
        PrintNotImplemented("BindVertexBuffers");
    }

    void BindIndexBuffer(CommandBufferHandle cmd, BufferHandle buffer, sizeT offset)
    {
        PrintNotImplemented("BindIndexBuffer");
    }
}

#endif
