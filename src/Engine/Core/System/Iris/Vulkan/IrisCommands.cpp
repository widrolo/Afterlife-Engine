#if GPU_BACKEND == GPU_VULKAN

#include <Engine/Core/System/Iris.h>
#include <Engine/Math/Vector.h>

#include "IrisGlobals.h"
#include "Engine/Util/Log.h"
#include "Helpers/Helpers.h"

namespace Iris
{
    void SetViewport(CommandBufferHandle cmd, const Viewport& viewport)
    {
        if (cmd == 0 || cmd > loadedCommandBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid command buffer handle, refusing to set viewport!");
            return;
        }
        VkViewport vp{};
        vp.x = viewport.pos.x;
        vp.y = viewport.pos.y;
        vp.width = viewport.extent.x;
        vp.height = viewport.extent.y;
        vp.minDepth = viewport.minDepth;
        vp.maxDepth = viewport.maxDepth;
        vkCmdSetViewport(GetCurrentCmdBuff(cmd), 0, 1, &vp);

        VkRect2D scissor{};
        scissor.offset.x = viewport.pos.x;
        scissor.offset.y = viewport.pos.y;
        scissor.extent.width = viewport.extent.x;
        scissor.extent.height = viewport.extent.y;
        vkCmdSetScissor(GetCurrentCmdBuff(cmd), 0, 1, &scissor);
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