#include <Engine/EngineDefines.h>
#if GPU_BACKEND == GPU_VULKAN

#include "VulkanImages.h"
#include <vulkan/vulkan.h>

#include "VulkanHelpers.h"
#include "Engine/Util/Log.h"

bool SetupDepthImage(VulkanContext& ctx, VulkanStatistics& stat)
{
    VkImageCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.imageType = VK_IMAGE_TYPE_2D;
    info.extent = { (uint32)EngineSettings::resolution.x, (uint32)EngineSettings::resolution.y, 1 };
    info.mipLevels = 1;
    info.arrayLayers = 1;
    info.format = FindBestDepthFormat(ctx);
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    info.samples = VK_SAMPLE_COUNT_1_BIT;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    VmaAllocationInfo allocationInfo{};

    auto res = vmaCreateImage(ctx.vcore.vmaAllocator, &info, &allocInfo, &ctx.screen.depthImage, &ctx.screen.depthAllocation,
        &allocationInfo);

    stat.vramUsage += allocationInfo.size;

    if (!ParseVkResult(res))
    {
        WEngine::WLog::SetConsoleError();
        WEngine::WLog::ConsoleLog("Unable to create depth image.");
        return false;
    }

    VkImageViewCreateInfo depthViewInfo{};
    depthViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    depthViewInfo.image = ctx.screen.depthImage;
    depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    depthViewInfo.format = FindBestDepthFormat(ctx);
    depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    depthViewInfo.subresourceRange.baseMipLevel = 0;
    depthViewInfo.subresourceRange.levelCount = 1;
    depthViewInfo.subresourceRange.baseArrayLayer = 0;
    depthViewInfo.subresourceRange.layerCount = 1;

    vkCreateImageView(ctx.vcore.gpuDevice, &depthViewInfo, ctx.vcore.allocator, &ctx.screen.depthImageView);

    return true;
}

#endif