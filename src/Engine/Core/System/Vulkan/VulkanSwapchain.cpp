#include <Engine/EngineDefines.h>
#if GPU_BACKEND == GPU_VULKAN

#include "VulkanSwapchain.h"

#include "VulkanHelpers.h"

bool SetupSwapchain(VulkanContext& ctx, VulkanStatistics& stats)
{
    VkSurfaceCapabilitiesKHR capabilities{};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(ctx.vcore.gpuPhysicalDevice, ctx.screen.screen, &capabilities);

    uint32 fmtCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(ctx.vcore.gpuPhysicalDevice, ctx.screen.screen, &fmtCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(fmtCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(ctx.vcore.gpuPhysicalDevice, ctx.screen.screen, &fmtCount, formats.data());


    VkSwapchainCreateInfoKHR info{};
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.surface = ctx.screen.screen;
    info.minImageCount = capabilities.minImageCount + 1;

    info.imageFormat = FindBestSwapchainFormat(ctx);
    info.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    info.imageExtent = capabilities.currentExtent;
    info.imageArrayLayers = 1;
    info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    info.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    info.oldSwapchain = VK_NULL_HANDLE;

    auto res = vkCreateSwapchainKHR(ctx.vcore.gpuDevice, &info, ctx.vcore.allocator, &ctx.screen.swapchain);

    uint32 swapchainImageCount;
    vkGetSwapchainImagesKHR(ctx.vcore.gpuDevice, ctx.screen.swapchain, &swapchainImageCount, nullptr);
    ctx.screen.swapchainImages.resize(swapchainImageCount);
    vkGetSwapchainImagesKHR(ctx.vcore.gpuDevice, ctx.screen.swapchain, &swapchainImageCount, ctx.screen.swapchainImages.data());

    ctx.screen.imageAvailableSems.resize(swapchainImageCount);
    ctx.screen.renderFinishedSems.resize(swapchainImageCount);
    ctx.screen.endOfFrameFences.resize(swapchainImageCount);
    ctx.bufferGraveyard.resize(swapchainImageCount);

    stats.vramUsage += CalcTextureSize(4, EngineSettings::resolution.x, EngineSettings::resolution.y) * swapchainImageCount;

    VkSemaphoreCreateInfo semInfo{};
    semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    for (uint32 i = 0; i < swapchainImageCount; i++)
        vkCreateSemaphore(ctx.vcore.gpuDevice, &semInfo, ctx.vcore.allocator, &ctx.screen.imageAvailableSems[i]);
    for (uint32 i = 0; i < swapchainImageCount; i++)
        vkCreateSemaphore(ctx.vcore.gpuDevice, &semInfo, ctx.vcore.allocator, &ctx.screen.renderFinishedSems[i]);

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    for (uint32 i = 0; i < swapchainImageCount; i++)
        vkCreateFence(ctx.vcore.gpuDevice, &fenceInfo, ctx.vcore.allocator, &ctx.screen.endOfFrameFences[i]);

    ctx.screen.swapchainImageViews.resize(swapchainImageCount);
    for (uint32 i = 0; i < swapchainImageCount; i++)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = FindBestSwapchainFormat(ctx);
        viewInfo.image = ctx.screen.swapchainImages[i];
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        auto resImg = vkCreateImageView(ctx.vcore.gpuDevice, &viewInfo, ctx.vcore.allocator,
            &ctx.screen.swapchainImageViews[i]);

        if (!ParseVkResult(resImg))
            return false;
    }

    return ParseVkResult(res);
}

#endif