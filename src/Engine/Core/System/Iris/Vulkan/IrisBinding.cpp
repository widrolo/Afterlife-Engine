#if GPU_BACKEND == GPU_VULKAN

#include <Engine/Core/System/Iris.h>

using namespace Iris;

void BindGraphicsPipeline(CommandBufferHandle cmd, GraphicsPipelineHandle pipeline)
{

}

void BindComputePipeline(CommandBufferHandle cmd, ComputePipelineHandle  pipeline)
{

}

void BindResourceTable(CommandBufferHandle cmd, uint32 slot, ResourceTableHandle table)
{

}

void SetPushConstants(CommandBufferHandle cmd, const byte* data, sizeT size)
{

}

void BindVertexBuffers(CommandBufferHandle cmd, uint32 firstBinding, const BufferHandle* buffers,
    const sizeT* offsets, sizeT count)
{

}

void BindIndexBuffer(CommandBufferHandle cmd, BufferHandle buffer, sizeT offset)
{

}

#endif
