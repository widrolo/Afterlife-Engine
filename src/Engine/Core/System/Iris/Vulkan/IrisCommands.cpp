#if GPU_BACKEND == GPU_VULKAN

#include <Engine/Core/System/Iris.h>
#include <Engine/Math/Vector.h>

using namespace Iris;

void SetViewport(CommandBufferHandle cmd, const Viewport& viewport)
{

}

void SetScissor(CommandBufferHandle cmd, const Scissor& scissor)
{

}

void Draw(CommandBufferHandle cmd, sizeT vertexCount, sizeT instanceCount, sizeT firstVertex, sizeT firstInstance)
{

}

void DrawIndexed(CommandBufferHandle cmd, sizeT indexCount, sizeT instanceCount, sizeT firstIndex, sizeT vertexOffset,
    sizeT firstInstance)
{

}

void DrawIndirect(CommandBufferHandle cmd, BufferHandle argBuffer, sizeT offset, sizeT drawCount, sizeT stride)
{

}

void DrawIndexedIndirect(CommandBufferHandle cmd, BufferHandle argBuffer, sizeT offset, sizeT drawCount, sizeT stride)
{

}

void Dispatch(CommandBufferHandle cmd, sizeT groupCountX, sizeT groupCountY, sizeT groupCountZ)
{

}

void DispatchIndirect(CommandBufferHandle cmd, BufferHandle argBuffer, sizeT offset)
{

}

void CopyBufferToBuffer(CommandBufferHandle cmd, BufferHandle dst, sizeT dstOffset, BufferHandle src, sizeT srcOffset,
    sizeT size)
{

}

void CopyBufferToTexture(CommandBufferHandle cmd, BufferHandle src, sizeT srcOffset, TextureHandle dst,
    WEngine::Vector2 extent)
{

}

#endif