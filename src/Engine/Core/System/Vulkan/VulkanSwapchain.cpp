#include <Engine/EngineDefines.h>
#if GPU_BACKEND == GPU_VULKAN

#include "VulkanSwapchain.h"

#include "VulkanCommands.h"
#include "VulkanHelpers.h"

VkPresentModeKHR FindBestPresentMode(VulkanContext &ctx)
{
    uint32_t count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(ctx.vcore.gpuPhysicalDevice, ctx.screen.screen, &count, nullptr);

    std::vector<VkPresentModeKHR> modes(count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(ctx.vcore.gpuPhysicalDevice, ctx.screen.screen, &count, modes.data());

    bool hasMailbox = false;

    for (VkPresentModeKHR mode : modes)
    {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            hasMailbox = true;
            break;
        }
    }

    if (hasMailbox)
        return VK_PRESENT_MODE_MAILBOX_KHR;

    return VK_PRESENT_MODE_FIFO_KHR; // always guaranteed
}

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
    info.presentMode = FindBestPresentMode(ctx);
    info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    info.oldSwapchain = VK_NULL_HANDLE;

    auto res = vkCreateSwapchainKHR(ctx.vcore.gpuDevice, &info, ctx.vcore.allocator, &ctx.screen.swapchain);

    vkGetSwapchainImagesKHR(ctx.vcore.gpuDevice, ctx.screen.swapchain, &ctx.screen.swapchainImageCount, nullptr);
    ctx.displayTarget.targetImages.resize(ctx.screen.swapchainImageCount);
    vkGetSwapchainImagesKHR(ctx.vcore.gpuDevice, ctx.screen.swapchain, &ctx.screen.swapchainImageCount,
        ctx.displayTarget.targetImages.data());

    ctx.bufferGraveyard.resize(ctx.screen.swapchainImageCount);

    stats.vramUsage += CalcTextureSize(4, EngineSettings::resolution.x, EngineSettings::resolution.y) * ctx.screen.swapchainImageCount;

    PopulateSemsAndFences(ctx, ctx.displayTarget);

    ctx.displayTarget.resolution = EngineSettings::resolution;

    ctx.displayTarget.targetImageViews.resize(ctx.screen.swapchainImageCount);
    for (sizeT i = 0; i < ctx.screen.swapchainImageCount; i++)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = FindBestSwapchainFormat(ctx);
        viewInfo.image = ctx.displayTarget.targetImages[i];
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        auto resImg = vkCreateImageView(ctx.vcore.gpuDevice, &viewInfo, ctx.vcore.allocator,
            &ctx.displayTarget.targetImageViews[i]);

        if (!ParseVkResult(resImg))
            return false;
    }

    return ParseVkResult(res);
}

#endif