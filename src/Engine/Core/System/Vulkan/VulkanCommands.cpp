#include <Engine/EngineDefines.h>
#if GPU_BACKEND == GPU_VULKAN
#include "VulkanCommands.h"

#include "VulkanHelpers.h"
#include "VulkanImages.h"
#include "VulkanPipeline.h"
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

    for (uint32 i = 0; i < ctx.screen.swapchainImageCount; i++)
        ctx.displayTarget.cmdBuffs[i] = CreateCommandBuffer(ctx);

    // this is just so i dont get slapped by a cock in the face if i forget to do it in the render handler
    ctx.currentRenderTarget = &ctx.displayTarget;
    return true;
}

Vulkan_RenderTarget CreateRenderTarget(VulkanContext &ctx, VulkanStatistics &stat, const WEngine::Vector2& resolution)
{
    Vulkan_RenderTarget target;
    VkFormat format = FindBestSwapchainFormat(ctx);

    target.resolution = resolution;

    target.cmdBuffs.resize(ctx.screen.swapchainImageCount);
    target.targetImages.resize(ctx.screen.swapchainImageCount);
    target.targetImageViews.resize(ctx.screen.swapchainImageCount);
    target.targetSampler.resize(ctx.screen.swapchainImageCount);
    target.targetImageAlloc.resize(ctx.screen.swapchainImageCount);

    for (uint32 i = 0; i < ctx.screen.swapchainImageCount; i++)
    {
        CreateImage(ctx, stat, resolution, format, target.targetImages[i], target.targetImageViews[i],
            target.targetImageAlloc[i], false);

        target.cmdBuffs[i] = CreateCommandBuffer(ctx);

        VkDescriptorSet set;
        VkDescriptorSetAllocateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        info.descriptorPool = ctx.postProcessing.descriptorPool;
        info.descriptorSetCount = 1;
        info.pSetLayouts = &ctx.postProcessing.descriptorSetLayout;

        auto res = vkAllocateDescriptorSets(ctx.vcore.gpuDevice, &info, &set);

        if (!ParseVkResult(res))
        {
            WEngine::WLog::SetConsoleError();
            WEngine::WLog::ConsoleLog("Unable to create descriptor set for render target!");
            return {};
        }

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 1.0f;

        res = vkCreateSampler(ctx.vcore.gpuDevice, &samplerInfo, ctx.vcore.allocator, &target.targetSampler[i]);

        if (!ParseVkResult(res))
        {
            WEngine::WLog::SetConsoleError();
            WEngine::WLog::ConsoleLog("Unable to create sampler for render target!");
            return {};
        }

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageView   = target.targetImageViews[i];
        imageInfo.sampler     = target.targetSampler[i];
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = set;
        write.dstBinding = 0;
        write.dstArrayElement = 0;
        write.descriptorCount = 1;
        write.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(ctx.vcore.gpuDevice, 1, &write, 0, nullptr);
    }

    PopulateSemsAndFences(ctx, target);

    return target;
}

void PopulateSemsAndFences(VulkanContext &ctx, Vulkan_RenderTarget &rt)
{
    rt.imageAvailableSems.resize(ctx.screen.swapchainImageCount);
    rt.renderFinishedSems.resize(ctx.screen.swapchainImageCount);
    rt.endOfFrameFences.resize(ctx.screen.swapchainImageCount);

    VkSemaphoreCreateInfo semInfo{};
    semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    for (uint32 i = 0; i < ctx.screen.swapchainImageCount; i++)
        vkCreateSemaphore(ctx.vcore.gpuDevice, &semInfo, ctx.vcore.allocator, &rt.imageAvailableSems[i]);
    for (uint32 i = 0; i < ctx.screen.swapchainImageCount; i++)
        vkCreateSemaphore(ctx.vcore.gpuDevice, &semInfo, ctx.vcore.allocator, &rt.renderFinishedSems[i]);

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    for (uint32 i = 0; i < ctx.screen.swapchainImageCount; i++)
        vkCreateFence(ctx.vcore.gpuDevice, &fenceInfo, ctx.vcore.allocator, &rt.endOfFrameFences[i]);
}

#endif
