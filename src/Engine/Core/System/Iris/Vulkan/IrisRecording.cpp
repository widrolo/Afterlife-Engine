#if GPU_BACKEND == GPU_VULKAN

#include <Engine/Core/System/Iris.h>

using namespace Iris;

CommandBufferHandle BeginCommandBuffer(QueueType queue = QueueType::Graphics)
{

}

void EndCommandBuffer(CommandBufferHandle cmd)
{

}

void SubmitCommandBuffer(CommandBufferHandle cmd)
{

}

void SubmitCommandBuffers(const CommandBufferHandle* cmds, sizeT count)
{

}

void BeginRenderPass(CommandBufferHandle cmd, const RenderPassBeginDesc& desc)
{

}

void EndRenderPass(CommandBufferHandle cmd)
{

}

void BeginComputePass(CommandBufferHandle cmd)
{

}

void EndComputePass(CommandBufferHandle cmd)
{

}

void BeginCopyPass(CommandBufferHandle cmd)
{

}

void EndCopyPass(CommandBufferHandle cmd)
{

}

#endif