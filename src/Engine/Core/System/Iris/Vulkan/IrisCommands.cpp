#if GPU_BACKEND == GPU_VULKAN

#include <Engine/Core/System/Iris.h>
#include <Engine/Math/Vector.h>

#include "Helpers/Helpers.h"

namespace Iris
{
    void SetViewport(CommandBufferHandle cmd, const Viewport& viewport)
    {
        PrintNotImplemented("SetViewport");
    }

    void SetScissor(CommandBufferHandle cmd, const Scissor& scissor)
    {
        PrintNotImplemented("SetScissor");
    }

    void Draw(CommandBufferHandle cmd, sizeT vertexCount, sizeT instanceCount, sizeT firstVertex, sizeT firstInstance)
    {
        PrintNotImplemented("Draw");
    }

    void DrawIndexed(CommandBufferHandle cmd, sizeT indexCount, sizeT instanceCount, sizeT firstIndex, sizeT vertexOffset,
        sizeT firstInstance)
    {
        PrintNotImplemented("DrawIndexed");
    }

    void DrawIndirect(CommandBufferHandle cmd, BufferHandle argBuffer, sizeT offset, sizeT drawCount, sizeT stride)
    {
        PrintNotImplemented("DrawIndirect");
    }

    void DrawIndexedIndirect(CommandBufferHandle cmd, BufferHandle argBuffer, sizeT offset, sizeT drawCount, sizeT stride)
    {
        PrintNotImplemented("DrawIndexedIndirect");
    }

    void Dispatch(CommandBufferHandle cmd, sizeT groupCountX, sizeT groupCountY, sizeT groupCountZ)
    {
        PrintNotImplemented("Dispatch");
    }

    void DispatchIndirect(CommandBufferHandle cmd, BufferHandle argBuffer, sizeT offset)
    {
        PrintNotImplemented("DispatchIndirect");
    }

    void CopyBufferToBuffer(CommandBufferHandle cmd, BufferHandle dst, sizeT dstOffset, BufferHandle src, sizeT srcOffset,
        sizeT size)
    {
        PrintNotImplemented("CopyBufferToBuffer");
    }

    void CopyBufferToTexture(CommandBufferHandle cmd, BufferHandle src, sizeT srcOffset, TextureHandle dst,
        WEngine::Vector2 extent)
    {
        PrintNotImplemented("CopyBufferToTexture");
    }
}

#endif