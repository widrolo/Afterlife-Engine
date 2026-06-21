#include <Engine/EngineDefines.h>
#if GPU_BACKEND == GPU_VULKAN
#include "VulkanCommands.h"

#include "VulkanHelpers.h"
#include "VulkanImages.h"
#include "Engine/Util/Log.h"

bool SetupDrawCommandPool(VulkanContext& ctx)
{
    VkCommandPoolCreateInfo commandPoolInfo{};
    commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolInfo.queueFamilyIndex = ctx.queues.primaryDrawQueueFamilyIndex;
    commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    auto res = vkCreateCommandPool(ctx.vcore.gpuDevice, &commandPoolInfo, ctx.vcore.allocator, &ctx.commandPool);

    if (!ParseVkResult(res))
    {
        WEngine::WLog::SetConsoleWarning();
        WEngine::WLog::ConsoleLog("Unable to create command buffer!");
        return false;
    }
    return true;
}

VkCommandBuffer CreateCommandBuffer(const VulkanContext &ctx)
{
    VkCommandBuffer cmdBuff;

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = ctx.commandPool;
    allocInfo.commandBufferCount = 1;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    auto res = vkAllocateCommandBuffers(ctx.vcore.gpuDevice, &allocInfo, &cmdBuff);

    if (!ParseVkResult(res))
    {
        WEngine::WLog::SetConsoleWarning();
        WEngine::WLog::ConsoleLog("Unable to create command buffer!");
        return VK_NULL_HANDLE;
    }

    return cmdBuff;
}

bool SetupDisplayRenderTarget(VulkanContext &ctx, VulkanStatistics& stat)
{
    //VkFormat format = FindBestSwapchainFormat(ctx);

    if (!SetupDrawCommandPool(ctx))
        return false;

    ctx.displayTarget.cmdBuffs.resize(ctx.screen.swapchainImageCount);
    //ctx.displayTarget.targetImages.resize(ctx.screen.swapchainImages.size());
    //ctx.displayTarget.targetImageViews.resize(ctx.screen.swapchainImages.size());
    //ctx.displayTarget.targetImageAlloc.resize(ctx.screen.swapchainImages.size());

    for (uint32 i = 0; i < ctx.screen.swapchainImageCount; i++)
    {
        //CreateImage(ctx, stat, EngineSettings::resolution, format, ctx.displayTarget.targetImages[i],
        //    ctx.displayTarget.targetImageViews[i], ctx.displayTarget.targetImageAlloc[i], false);
        ctx.displayTarget.cmdBuffs[i] = CreateCommandBuffer(ctx);
    }

    // this is just so i dont get slapped by a cock in the face if i forget to do it in the render handler
    ctx.currentRenderTarget = &ctx.displayTarget;
    return true;
}

#endif
