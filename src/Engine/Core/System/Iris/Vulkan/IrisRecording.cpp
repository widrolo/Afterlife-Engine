#if GPU_BACKEND == GPU_VULKAN

#include <Engine/Core/System/Iris.h>

#include "IrisGlobals.h"
#include "Engine/Util/Log.h"
#include "Helpers/Helpers.h"
#include "Helpers/Queues.h"

namespace Iris
{
    CommandBufferHandle CreateCommandBuffer(QueueType queue)
    {
        Vulkan_CmdBuff entry{};
        entry.queue = QueueFor(queue);

        const uint32 frameCount = (uint32)framePools.size();

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        entry.commandBuffers.resize(frameCount);
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
        }

        loadedCommandBuffers.push_back(entry);
        return loadedCommandBuffers.size();
    }

    void BeginCommandBuffer(CommandBufferHandle cmd)
    {
        if (cmd == 0 || cmd > loadedCommandBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid command buffer handle, refusing to end!");
            return;
        }

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(GetCurrentCmdBuff(cmd), &beginInfo);
    }

    void EndCommandBuffer(CommandBufferHandle cmd)
    {
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
        if (cmd == 0 || cmd > loadedCommandBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid command buffer handle, refusing to submit!");
            return;
        }

        const uint32 slot = commandBufferFrameIndex % (uint32)framePools.size();
        const Vulkan_CmdBuff& cmdBuff = loadedCommandBuffers[cmd - 1];

        const VkSemaphore imageAvailable = displayTarget.imageAvailableSems[screen.currentFrame];
        const VkSemaphore renderFinished  = displayTarget.renderFinishedSems[screen.currentFrame];
        const VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        VkSubmitInfo submit{};
        submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit.waitSemaphoreCount   = 1;
        submit.pWaitSemaphores      = &imageAvailable;
        submit.pWaitDstStageMask    = &waitStage;
        submit.signalSemaphoreCount = 1;
        submit.pSignalSemaphores    = &renderFinished;
        submit.commandBufferCount   = 1;
        submit.pCommandBuffers      = &cmdBuff.commandBuffers[slot];

        vkQueueSubmit(cmdBuff.queue, 1, &submit, framePools[slot].fence);
    }

    // We conveniently ignore most of the attachment information of the description as of now because for now,
    // it doesnt bring anything to the table.
    void BeginRenderPass(CommandBufferHandle cmd, const RenderPassBeginDesc& desc)
    {
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

        VkImageMemoryBarrier colBarrier{};
        colBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        colBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colBarrier.image = displayTarget.targetImages[screen.currentFrame];
        colBarrier.srcAccessMask = 0;
        colBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        colBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        colBarrier.subresourceRange.baseMipLevel = 0;
        colBarrier.subresourceRange.levelCount = 1;
        colBarrier.subresourceRange.baseArrayLayer = 0;
        colBarrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(GetCurrentCmdBuff(cmd), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &colBarrier);

        // never trusting vulkan with things passed in as reference!
        VkImageMemoryBarrier depthBarrier = colBarrier;
        depthBarrier.image = screen.depthImage;
        depthBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depthBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        depthBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        vkCmdPipelineBarrier(GetCurrentCmdBuff(cmd), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, 0, 0, nullptr, 0, nullptr, 1, &depthBarrier);

        VkRenderingAttachmentInfo colorAttachmentInfo{};
        colorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        colorAttachmentInfo.clearValue = clearValues[0];
        colorAttachmentInfo.imageView = displayTarget.targetImageViews[screen.currentFrame];
        colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        VkRenderingAttachmentInfo depthAttachmentInfo{};
        depthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        depthAttachmentInfo.clearValue = clearValues[1];
        depthAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachmentInfo.imageView = screen.depthImageView;
        depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;


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
        if (cmd == 0 || cmd > loadedCommandBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid command buffer handle, refusing to end rendering!");
            return;
        }
        vkCmdEndRendering(GetCurrentCmdBuff(cmd));

        VkImageLayout newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkImageMemoryBarrier imgBarrier{};
        imgBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imgBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        imgBarrier.newLayout = newLayout;
        imgBarrier.image = displayTarget.targetImages[screen.currentFrame];
        imgBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        imgBarrier.dstAccessMask = 0;

        imgBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imgBarrier.subresourceRange.baseMipLevel = 0;
        imgBarrier.subresourceRange.levelCount = 1;
        imgBarrier.subresourceRange.baseArrayLayer = 0;
        imgBarrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(GetCurrentCmdBuff(cmd), VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &imgBarrier);
    }

    void BeginComputePass(CommandBufferHandle cmd)
    {
        PrintNotImplemented("BeginComputePass");
    }

    void EndComputePass(CommandBufferHandle cmd)
    {
        PrintNotImplemented("EndComputePass");
    }

    // I mean copy passes technically arent needed in vulkan. This is just visual fluff
    void BeginCopyPass(CommandBufferHandle cmd)
    {
    }

    void EndCopyPass(CommandBufferHandle cmd)
    {
    }
}

#endif