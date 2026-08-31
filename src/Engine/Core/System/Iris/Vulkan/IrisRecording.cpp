#if GPU_BACKEND == GPU_VULKAN

#include <Engine/Core/System/Iris.h>

#include "IrisGlobals.h"
#include "Engine/Util/Log.h"
#include "Engine/Util/TimeAnalysis.h"
#include "Helpers/Helpers.h"
#include "Helpers/Queues.h"

namespace Iris
{
    CommandBufferHandle CreateCommandBuffer(QueueType queue)
    {
        WEngine::TimeSample sample("[Iris]CreateCommandBuffer");
        Vulkan_CmdBuff entry{};
        entry.queue = QueueFor(queue);

        const uint32 frameCount = (uint32)framePools.size();

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        entry.commandBuffers.resize(frameCount);
        entry.fences.resize(frameCount);
        entry.signalSems.resize(frameCount);
        for (uint32 i = 0; i < frameCount; ++i)
        {
            allocInfo.commandPool = framePools[i].pool[(uint8)queue];
            auto res = vkAllocateCommandBuffers(vcore.gpuDevice, &allocInfo, &entry.commandBuffers[i]);
            if (!ParseVkResult(res))
            {
                WEngine::WLog::SetConsoleError();
                WEngine::WLog::ConsoleLog("Unable to create command buffer");
                return 0;
            }

            VkFenceCreateInfo fenceInfo{};
            fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // so the first frame doesn't block forever
            res = vkCreateFence(vcore.gpuDevice, &fenceInfo, vcore.allocator, &entry.fences[i]);
            if (!ParseVkResult(res))
            {
                WEngine::WLog::SetConsoleError();
                WEngine::WLog::ConsoleLog("Unable to create command buffer fence");
                return 0;
            }

            VkSemaphoreCreateInfo semInfo{};
            semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            res = vkCreateSemaphore(vcore.gpuDevice, &semInfo, vcore.allocator, &entry.signalSems[i]);
            if (!ParseVkResult(res))
            {
                WEngine::WLog::SetConsoleError();
                WEngine::WLog::ConsoleLog("Unable to create command buffer semaphore");
                return 0;
            }
        }

        loadedCommandBuffers.push_back(entry);
        return loadedCommandBuffers.size();
    }

    CopyBufferHandle CreateCopyBuffer()
    {
        WEngine::TimeSample sample("[Iris]CreateCopyBuffer");
        Vulkan_CopyBuff buff{};
        buff.queue = QueueFor(QueueType::Copy);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = QueueFamilyFor(QueueType::Copy);
        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        auto res = vkCreateCommandPool(vcore.gpuDevice, &poolInfo, vcore.allocator, &buff.pool);
        if (!ParseVkResult(res))
        {
            WEngine::WLog::SetConsoleError();
            WEngine::WLog::ConsoleLog("Unable to create copy buffer pool!");
            return 0;
        }

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;
        allocInfo.commandPool = buff.pool;

        res = vkAllocateCommandBuffers(vcore.gpuDevice, &allocInfo, &buff.commandBuffer);
        if (!ParseVkResult(res))
        {
            WEngine::WLog::SetConsoleError();
            WEngine::WLog::ConsoleLog("Unable to create copy buffer!");
            return 0;
        }
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        res = vkCreateFence(vcore.gpuDevice, &fenceInfo, vcore.allocator, &buff.fence);
        if (!ParseVkResult(res))
        {
            WEngine::WLog::SetConsoleError();
            WEngine::WLog::ConsoleLog("Unable to create fence for the copy buffer!");
            return 0;
        }

        loadedCopyBuffers.push_back(buff);
        return loadedCopyBuffers.size();
    }

    void BeginCommandBuffer(CommandBufferHandle cmd)
    {
        WEngine::TimeSample sample("[Iris]BeginCommandBuffer");
        if (cmd == 0 || cmd > loadedCommandBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid command buffer handle, refusing to begin!");
            return;
        }

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(GetCurrentCmdBuff(cmd), &beginInfo);
    }

    void EndCommandBuffer(CommandBufferHandle cmd)
    {
        WEngine::TimeSample sample("[Iris]EndCommandBuffer");
        if (cmd == 0 || cmd > loadedCommandBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid command buffer handle, refusing to end!");
            return;
        }
        vkEndCommandBuffer(GetCurrentCmdBuff(cmd));
    }

    void SubmitCommandBuffer(CommandBufferHandle cmd)
    {
        WEngine::TimeSample sample("[Iris]SubmitCommandBuffer");
        if (cmd == 0 || cmd > loadedCommandBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid command buffer handle, refusing to submit!");
            return;
        }

        const uint32 slot = commandBufferFrameIndex % (uint32)framePools.size();
        const Vulkan_CmdBuff& cmdBuff = loadedCommandBuffers[cmd - 1];

        const VkSemaphore waitSemaphore = (lastSubmittedSignalSem == VK_NULL_HANDLE)
            ? displayTarget.imageAvailableSems[screen.currentFrame]
            : lastSubmittedSignalSem;
        const VkSemaphore renderFinished  = cmdBuff.signalSems[slot];
        const VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        VkSubmitInfo submit{};
        submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit.waitSemaphoreCount   = 1;
        submit.pWaitSemaphores      = &waitSemaphore;
        submit.pWaitDstStageMask    = &waitStage;
        submit.signalSemaphoreCount = 1;
        submit.pSignalSemaphores    = &renderFinished;
        submit.commandBufferCount   = 1;
        submit.pCommandBuffers      = &cmdBuff.commandBuffers[slot];

        vkQueueSubmit(cmdBuff.queue, 1, &submit, cmdBuff.fences[slot]);

        lastSubmittedSignalSem = renderFinished;
    }

    // We conveniently ignore most of the attachment information of the description as of now because for now,
    // it doesnt bring anything to the table.
    void BeginRenderPass(CommandBufferHandle cmd, const RenderPassBeginDesc& desc)
    {
        WEngine::TimeSample sample("[Iris]BeginRenderPass");
        if (cmd == 0 || cmd > loadedCommandBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid command buffer handle, refusing to begin rendering!");
            return;
        }
        VkClearColorValue clearCol;
        WEngine::Colorf col = desc.colorAttachment.clearColor;
        clearCol = { col.red, col.green, col.blue, col.alpha };

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = clearCol;
        clearValues[1].depthStencil = { desc.depthStencil.clearDepth, desc.depthStencil.clearStencil };

        VkImage* colorImage = nullptr;
        VkImage* depthImage = nullptr;
        VkImageView* colorImageView = nullptr;
        VkImageView* depthImageView = nullptr;

        if (desc.framebuffer == 0 || desc.framebuffer > loadedRenderTargets.size())
        {
            currentlyRecording = nullptr;

            colorImage = &displayTarget.targetImages[screen.currentFrame];
            colorImageView = &displayTarget.targetImageViews[screen.currentFrame];
            depthImage = &screen.depthImage;
            depthImageView = &screen.depthImageView;
        }
        else
        {
            auto& rt = loadedRenderTargets[desc.framebuffer - 1];
            uint8 imageIndex = rt.lastUsedImage;
            rt.lastUsedImage = (uint8)((imageIndex + 1) % Vulkan_RenderTarget::maxIF);
            rt.currentImage = imageIndex;

            currentlyRecording = &rt;

            colorImage = &rt.targetImages[imageIndex];
            colorImageView = &rt.targetImageViews[imageIndex];
            depthImage = &rt.depthImages[imageIndex];
            depthImageView = &rt.depthImageViews[imageIndex];
        }
        bool hasDepth = *depthImage != nullptr;

        VkImageMemoryBarrier colBarrier{};
        colBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        colBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colBarrier.image = *colorImage;
        colBarrier.srcAccessMask = 0;
        colBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        colBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        colBarrier.subresourceRange.baseMipLevel = 0;
        colBarrier.subresourceRange.levelCount = 1;
        colBarrier.subresourceRange.baseArrayLayer = 0;
        colBarrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(GetCurrentCmdBuff(cmd), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &colBarrier);

        if (hasDepth)
        {
            // never trusting vulkan with things passed in as reference!
            VkImageMemoryBarrier depthBarrier = colBarrier;
            depthBarrier.image = *depthImage;
            depthBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            depthBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            depthBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            vkCmdPipelineBarrier(GetCurrentCmdBuff(cmd), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, 0, 0, nullptr, 0, nullptr, 1, &depthBarrier);
        }
        VkRenderingAttachmentInfo colorAttachmentInfo{};
        colorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        colorAttachmentInfo.clearValue = clearValues[0];
        colorAttachmentInfo.imageView = *colorImageView;
        colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        VkRenderingAttachmentInfo depthAttachmentInfo{};
        depthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        if (hasDepth)
        {
            depthAttachmentInfo.clearValue = clearValues[1];
            depthAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            depthAttachmentInfo.imageView = *depthImageView;
            depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        }
        else
        {

        }

        VkRenderingInfo renderingInfo{};
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderingInfo.renderArea.offset = {0, 0};
        renderingInfo.renderArea.extent = {1920, 1080}; // temporary max resolution until i can figure this out.
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachmentInfo;
        renderingInfo.pDepthAttachment = &depthAttachmentInfo;

        vkCmdBeginRendering(GetCurrentCmdBuff(cmd), &renderingInfo);
    }

    void EndRenderPass(CommandBufferHandle cmd)
    {
        WEngine::TimeSample sample("[Iris]EndRenderPass");
        if (cmd == 0 || cmd > loadedCommandBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid command buffer handle, refusing to end rendering!");
            return;
        }
        vkCmdEndRendering(GetCurrentCmdBuff(cmd));

        VkImageMemoryBarrier imgBarrier{};
        imgBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imgBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        imgBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        imgBarrier.dstAccessMask = 0;

        if (currentlyRecording == nullptr)
        {
            imgBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            imgBarrier.image = displayTarget.targetImages[screen.currentFrame];
        }
        else
        {
            imgBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imgBarrier.image = currentlyRecording->targetImages[currentlyRecording->currentImage];
        }

        imgBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imgBarrier.subresourceRange.baseMipLevel = 0;
        imgBarrier.subresourceRange.levelCount = 1;
        imgBarrier.subresourceRange.baseArrayLayer = 0;
        imgBarrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(GetCurrentCmdBuff(cmd), VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &imgBarrier);

        // The depth buffer gets the same treatment as the color one, otherwise the
        // depth descriptor sets (written as SHADER_READ_ONLY_OPTIMAL) are sampled
        // from an image still stuck in DEPTH_STENCIL_ATTACHMENT_OPTIMAL.
        if (currentlyRecording != nullptr && currentlyRecording->hasDepth)
        {
            VkImageMemoryBarrier depthBarrier{};
            depthBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            depthBarrier.oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            depthBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            depthBarrier.image = currentlyRecording->depthImages[currentlyRecording->currentImage];
            depthBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            depthBarrier.dstAccessMask = 0;

            depthBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            depthBarrier.subresourceRange.baseMipLevel = 0;
            depthBarrier.subresourceRange.levelCount = 1;
            depthBarrier.subresourceRange.baseArrayLayer = 0;
            depthBarrier.subresourceRange.layerCount = 1;

            vkCmdPipelineBarrier(GetCurrentCmdBuff(cmd), VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                0, 0, nullptr, 0, nullptr, 1, &depthBarrier);
        }
    }

    void BeginComputePass(CommandBufferHandle cmd)
    {
        WEngine::TimeSample sample("[Iris]BeginComputePass");
        PrintNotImplemented("BeginComputePass");
    }

    void EndComputePass(CommandBufferHandle cmd)
    {
        WEngine::TimeSample sample("[Iris]EndComputePass");
        PrintNotImplemented("EndComputePass");
    }

    void BeginCopyPass(CopyBufferHandle cmd)
    {
        WEngine::TimeSample sample("[Iris]BeginCopyPass");
        if (cmd == 0 || cmd > loadedCopyBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid copy buffer handle, refusing to begin!");
            return;
        }

        const Vulkan_CopyBuff& copyBuff = loadedCopyBuffers[cmd - 1];

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkResetFences(vcore.gpuDevice, 1, &copyBuff.fence);
        vkBeginCommandBuffer(copyBuff.commandBuffer, &beginInfo);
    }

    void EndCopyPass(CopyBufferHandle cmd)
    {
        WEngine::TimeSample sample("[Iris]EndCopyPass");
        if (cmd == 0 || cmd > loadedCopyBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid copy buffer handle, refusing to end!");
            return;
        }

        const Vulkan_CopyBuff& copyBuff = loadedCopyBuffers[cmd - 1];
        vkEndCommandBuffer(copyBuff.commandBuffer);

        VkSubmitInfo submit{};
        submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit.commandBufferCount   = 1;
        submit.pCommandBuffers      = &copyBuff.commandBuffer;

        auto res = vkQueueSubmit(copyBuff.queue, 1, &submit, copyBuff.fence);

        if (!ParseVkResult(res))
        {
            WEngine::WLog::SetConsoleError();
            WEngine::WLog::ConsoleLog("Copy submit failed.!");
        }
    }

    bool IsCopyPassDone(CopyBufferHandle cmd)
    {
        WEngine::TimeSample sample("[Iris]IsCopyPassDone");
        if (cmd == 0 || cmd > loadedCopyBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid copy buffer handle, refusing to end!");
            return false; // we should rather stall the copy process rather than overriding it mid-copy
        }

        const Vulkan_CopyBuff& copyBuff = loadedCopyBuffers[cmd - 1];
        if (vkGetFenceStatus(vcore.gpuDevice, copyBuff.fence) == VK_SUCCESS)
            return true;
        return false;
    }
}

#endif