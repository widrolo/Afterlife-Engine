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
        if (cmd == 0 || cmd > loadedCommandBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid command buffer handle, refusing to draw!");
            return;
        }
        vkCmdDraw(GetCurrentCmdBuff(cmd), vertexCount, instanceCount, firstVertex, firstInstance);
        stats.drawCallsThisFrame++;
    }

    void DrawIndexed(CommandBufferHandle cmd, sizeT indexCount, sizeT instanceCount, sizeT firstIndex, int32 vertexOffset,
        sizeT firstInstance)
    {
        if (cmd == 0 || cmd > loadedCommandBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid command buffer handle, refusing to draw indexed!");
            return;
        }
        vkCmdDrawIndexed(GetCurrentCmdBuff(cmd), indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
        stats.drawCallsThisFrame++;
    }

    void DrawIndirect(CommandBufferHandle cmd, BufferHandle argBuffer, sizeT offset, sizeT drawCount, sizeT stride)
    {
        if (cmd == 0 || cmd > loadedCommandBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid command buffer handle, refusing to draw indirect!");
            return;
        }
        if (argBuffer == 0 || argBuffer > loadedBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid argument buffer handle, refusing to draw indirect!");
            return;
        }
        const Vulkan_Buffer& buff = loadedBuffers[argBuffer - 1];
        vkCmdDrawIndirect(GetCurrentCmdBuff(cmd), buff.buffer, offset, drawCount, stride);
        stats.drawCallsThisFrame++;
    }

    void DrawIndexedIndirect(CommandBufferHandle cmd, BufferHandle argBuffer, sizeT offset, sizeT drawCount, sizeT stride)
    {
        if (cmd == 0 || cmd > loadedCommandBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid command buffer handle, refusing to draw indexed indirect!");
            return;
        }
        if (argBuffer == 0 || argBuffer > loadedBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid argument buffer handle, refusing to draw indexed indirect!");
            return;
        }
        const Vulkan_Buffer& buff = loadedBuffers[argBuffer - 1];
        vkCmdDrawIndexedIndirect(GetCurrentCmdBuff(cmd), buff.buffer, offset, drawCount, stride);
        stats.drawCallsThisFrame++;
    }

    void Dispatch(CommandBufferHandle cmd, sizeT groupCountX, sizeT groupCountY, sizeT groupCountZ)
    {
        if (cmd == 0 || cmd > loadedCommandBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid command buffer handle, refusing to dispatch!");
            return;
        }
        vkCmdDispatch(GetCurrentCmdBuff(cmd), groupCountX, groupCountY, groupCountZ);
        stats.drawCallsThisFrame++;
    }

    void DispatchIndirect(CommandBufferHandle cmd, BufferHandle argBuffer, sizeT offset)
    {
        if (cmd == 0 || cmd > loadedCommandBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid command buffer handle, refusing to dispatch indirect!");
            return;
        }
        if (argBuffer == 0 || argBuffer > loadedBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid argument buffer handle, refusing to dispatch indirect!");
            return;
        }
        const Vulkan_Buffer& buff = loadedBuffers[argBuffer - 1];
        vkCmdDispatchIndirect(GetCurrentCmdBuff(cmd), buff.buffer, offset);
        stats.drawCallsThisFrame++;
    }

    void CopyBufferToBuffer(CommandBufferHandle cmd, BufferHandle dst, sizeT dstOffset, BufferHandle src, sizeT srcOffset,
        sizeT size)
    {
        if (cmd == 0 || cmd > loadedCommandBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid command buffer handle, refusing to copy buffer to buffer!");
            return;
        }
        if (dst == 0 || dst > loadedBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid destination buffer handle, refusing to buffer to buffer!");
            return;
        }
        if (src == 0 || src > loadedBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid source buffer handle, refusing to buffer to buffer!");
            return;
        }

        const Vulkan_Buffer& srcBuff = loadedBuffers[src - 1];
        const Vulkan_Buffer& dstBuff = loadedBuffers[dst - 1];

        VkBufferCopy copyInfo{};
        copyInfo.dstOffset = dstOffset;
        copyInfo.srcOffset = srcOffset;
        copyInfo.size = size;

        vkCmdCopyBuffer(GetCurrentCmdBuff(cmd), srcBuff.buffer, dstBuff.buffer, 1, &copyInfo);
    }

    void CopyBufferToTexture(CommandBufferHandle cmd, BufferHandle src, sizeT srcOffset, TextureHandle dst,
        WEngine::Vector2 extent)
    {
        if (cmd == 0 || cmd > loadedCommandBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid command buffer handle, refusing to copy buffer to texture!");
            return;
        }
        if (dst == 0 || dst > loadedTextures.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid destination texture handle, refusing to buffer to texture!");
            return;
        }
        if (src == 0 || src > loadedBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid source buffer handle, refusing to buffer to texture!");
            return;
        }

        const Vulkan_Buffer& srcBuff = loadedBuffers[src - 1];
        const Vulkan_Texture& dstImg = loadedTextures[dst - 1];

        BC fmt;

        switch (dstImg.format)
        {
            case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
                fmt = BC::BC1;
                break;
            case VK_FORMAT_BC4_UNORM_BLOCK:
                fmt = BC::BC4;
                break;
            case VK_FORMAT_BC5_UNORM_BLOCK:
                fmt = BC::BC5;
                break;
            default:
                // Yeah, everything is BC around here. Were not doing soy boy uncompressed stuff around here.
                WEngine::WLog::SetConsoleWarning();
                WEngine::WLog::ConsoleLog("Invalid image format, must be BC1/4/5! Refusing to copy buffer to image!");
                return;
        }

        VkImageMemoryBarrier pipeBarrier{};
        pipeBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        pipeBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        pipeBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        pipeBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        pipeBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        pipeBarrier.image = dstImg.image;
        pipeBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        pipeBarrier.subresourceRange.baseMipLevel = 0;
        pipeBarrier.subresourceRange.levelCount = dstImg.mipCount;
        pipeBarrier.subresourceRange.baseArrayLayer = 0;
        pipeBarrier.subresourceRange.layerCount = 1;
        pipeBarrier.srcAccessMask = 0;
        pipeBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        vkCmdPipelineBarrier(GetCurrentCmdBuff(cmd), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
            0, 0, nullptr, 0, nullptr, 1, &pipeBarrier);

        std::vector<VkBufferImageCopy> regions;
        VkDeviceSize offset = 0;
        uint32 mw = dstImg.width;
        uint32 mh = dstImg.height;

        for (uint32 mip = 0; mip < dstImg.mipCount; ++mip)
        {
            VkBufferImageCopy region{};
            region.bufferOffset = offset;
            region.bufferRowLength = 0;
            region.bufferImageHeight = 0;
            region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.mipLevel = mip;
            region.imageSubresource.baseArrayLayer = 0;
            region.imageSubresource.layerCount = 1;
            region.imageExtent = { mw, mh, 1 };
            regions.push_back(region);
            offset += BCMipSize(mw, mh, fmt);
            mw = std::max(1u, mw / 2);
            mh = std::max(1u, mh / 2);
        }

        vkCmdCopyBufferToImage(GetCurrentCmdBuff(cmd), srcBuff.buffer, dstImg.image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, regions.size(), regions.data());

        VkImageMemoryBarrier postBarrier = pipeBarrier;
        postBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        postBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        postBarrier.subresourceRange.levelCount = dstImg.mipCount;
        postBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        postBarrier.dstAccessMask = 0;

        // This must transition to bottom of pipe, not fragment. This is not on a drawing queue!!
        vkCmdPipelineBarrier(GetCurrentCmdBuff(cmd), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            0, 0, nullptr, 0, nullptr, 1, &postBarrier);
    }
}

#endif