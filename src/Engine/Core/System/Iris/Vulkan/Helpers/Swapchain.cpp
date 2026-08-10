#if GPU_BACKEND == GPU_VULKAN
#include "Swapchain.h"

#include "Helpers.h"
#include "Sync.h"
#include "Engine/EngineDefines.h"
#include "Engine/Core/System/Iris/Vulkan/IrisGlobals.h"
#include "Engine/Util/Log.h"
#include "Engine/WTL/vector.h"

VkPresentModeKHR FindBestPresentMode()
{
    uint32_t count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(vcore.gpuPhysicalDevice, screen.screen, &count, nullptr);

    wtl::vector<VkPresentModeKHR> modes(count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(vcore.gpuPhysicalDevice, screen.screen, &count, modes.data());

    for (VkPresentModeKHR mode : modes)
    {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
            return VK_PRESENT_MODE_MAILBOX_KHR;
    }

    return VK_PRESENT_MODE_FIFO_KHR; // always guaranteed
}

bool SetupSwapchain()
{
    VkSurfaceCapabilitiesKHR capabilities{};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vcore.gpuPhysicalDevice, screen.screen, &capabilities);

    uint32 fmtCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(vcore.gpuPhysicalDevice, screen.screen, &fmtCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(fmtCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(vcore.gpuPhysicalDevice, screen.screen, &fmtCount, formats.data());


    VkSwapchainCreateInfoKHR info{};
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.surface = screen.screen;
    info.minImageCount = capabilities.minImageCount + 1;

    info.imageFormat = FindBestSwapchainFormat();
    info.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    info.imageExtent = capabilities.currentExtent;
    info.imageArrayLayers = 1;
    info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    info.presentMode = FindBestPresentMode();
    info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    info.oldSwapchain = VK_NULL_HANDLE;

    auto res = vkCreateSwapchainKHR(vcore.gpuDevice, &info, vcore.allocator, &screen.swapchain);

    vkGetSwapchainImagesKHR(vcore.gpuDevice, screen.swapchain, &screen.swapchainImageCount, nullptr);
    displayTarget.targetImages.resize(screen.swapchainImageCount);
    vkGetSwapchainImagesKHR(vcore.gpuDevice, screen.swapchain, &screen.swapchainImageCount, displayTarget.targetImages.data());

    bufferGraveyard.resize(screen.swapchainImageCount);

    sizeT size = CalcTextureSize(4, EngineSettings::resolution.x, EngineSettings::resolution.y) * screen.swapchainImageCount;
    stats.vramStats.total += size;
    stats.vramStats.framebuffers += size;

    PopulateSemsAndFences(displayTarget);

    displayTarget.resolution = EngineSettings::resolution;

    displayTarget.targetImageViews.resize(screen.swapchainImageCount);
    for (sizeT i = 0; i < screen.swapchainImageCount; i++)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = FindBestSwapchainFormat();
        viewInfo.image = displayTarget.targetImages[i];
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        auto resImg = vkCreateImageView(vcore.gpuDevice, &viewInfo, vcore.allocator,
            &displayTarget.targetImageViews[i]);

        if (!ParseVkResult(resImg))
            return false;
    }

    return ParseVkResult(res);
}

bool SetupDepthImage()
{
    VkImageCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.imageType = VK_IMAGE_TYPE_2D;
    info.extent = { (uint32)EngineSettings::resolution.x, (uint32)EngineSettings::resolution.y, 1 };
    info.mipLevels = 1;
    info.arrayLayers = 1;
    info.format = FindBestDepthFormat();
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    info.samples = VK_SAMPLE_COUNT_1_BIT;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    VmaAllocationInfo allocationInfo{};

    auto res = vmaCreateImage(vcore.vmaAllocator, &info, &allocInfo, &screen.depthImage, &screen.depthAllocation, &allocationInfo);

    stats.vramStats.total += allocationInfo.size;
    stats.vramStats.depthTextures += allocationInfo.size;

    if (!ParseVkResult(res))
    {
        WEngine::WLog::SetConsoleError();
        WEngine::WLog::ConsoleLog("Unable to create depth image.");
        return false;
    }

    VkImageViewCreateInfo depthViewInfo{};
    depthViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    depthViewInfo.image = screen.depthImage;
    depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    depthViewInfo.format = FindBestDepthFormat();
    depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    depthViewInfo.subresourceRange.baseMipLevel = 0;
    depthViewInfo.subresourceRange.levelCount = 1;
    depthViewInfo.subresourceRange.baseArrayLayer = 0;
    depthViewInfo.subresourceRange.layerCount = 1;

    res = vkCreateImageView(vcore.gpuDevice, &depthViewInfo, vcore.allocator, &screen.depthImageView);

    if (!ParseVkResult(res))
    {
        WEngine::WLog::SetConsoleError();
        WEngine::WLog::ConsoleLog("Unable to create depth image view.");
        return false;
    }

    return true;
}

#endif
