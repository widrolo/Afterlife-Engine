#include <Engine/EngineDefines.h>
#if GPU_BACKEND == GPU_VULKAN

#include "VulkanImages.h"

#include <cstring>
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

void CreateImage(VulkanContext& ctx, VulkanStatistics& stat, const WEngine::TextureInfoDDS &tex, VkImage& outImg,
                 VkImageView& outView, VmaAllocation& outAlloc)
{
    VkFormat format;
    switch (tex.format)
    {
        case BC::BC1: format = VK_FORMAT_BC1_RGBA_UNORM_BLOCK; break;
        case BC::BC4: format = VK_FORMAT_BC4_UNORM_BLOCK; break;
        case BC::BC5: format = VK_FORMAT_BC5_UNORM_BLOCK; break;
        default:      format = VK_FORMAT_UNDEFINED; break;
    }

    VkImageCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.imageType = VK_IMAGE_TYPE_2D;
    info.extent = {
        std::max(4u, (tex.width + 3u) & ~3u),
        std::max(4u, (tex.height + 3u) & ~3u),
        1
    };
    info.mipLevels = tex.mipCount;
    info.arrayLayers = 1;
    info.format = format;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    info.samples = VK_SAMPLE_COUNT_1_BIT;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    VmaAllocationInfo allocationInfo{};

    auto res = vmaCreateImage(ctx.vcore.vmaAllocator, &info, &allocInfo, &outImg, &outAlloc, &allocationInfo);

    stat.vramUsage += allocationInfo.size;

    if (!ParseVkResult(res))
    {
        WEngine::WLog::SetConsoleError();
        WEngine::WLog::ConsoleLog("Unable to create image.");
        return;
    }

    VkImageViewCreateInfo imageViewInfo{};
    imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewInfo.image = outImg;
    imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewInfo.format = format;
    imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewInfo.subresourceRange.baseMipLevel = 0;
    imageViewInfo.subresourceRange.levelCount = tex.mipCount;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.layerCount = 1;

    res = vkCreateImageView(ctx.vcore.gpuDevice, &imageViewInfo, ctx.vcore.allocator, &outView);

    if (!ParseVkResult(res))
    {
        WEngine::WLog::SetConsoleError();
        WEngine::WLog::ConsoleLog("Unable to create image view.");
        return;
    }
}

void CreateImageRenderTarget(VulkanContext &ctx, VulkanStatistics &stat, const WEngine::Vector2 &size, VkImage&outImg,
    VkImageView&outView, VmaAllocation&outAlloc)
{
    VkImageCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.imageType = VK_IMAGE_TYPE_2D;
    info.extent = { (uint32)size.x, (uint32)size.y, 1 };
    info.mipLevels = 1;
    info.arrayLayers = 1;
    info.format = FindBestSwapchainFormat(ctx);
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    info.samples = VK_SAMPLE_COUNT_1_BIT;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    VmaAllocationInfo allocationInfo{};

    auto res = vmaCreateImage(ctx.vcore.vmaAllocator, &info, &allocInfo, &outImg, &outAlloc, &allocationInfo);

    stat.vramUsage += allocationInfo.size;

    if (!ParseVkResult(res))
    {
        WEngine::WLog::SetConsoleError();
        WEngine::WLog::ConsoleLog("Unable to create image.");
        return;
    }

    VkImageViewCreateInfo imageViewInfo{};
    imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewInfo.image = outImg;
    imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewInfo.format = FindBestSwapchainFormat(ctx);
    imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewInfo.subresourceRange.baseMipLevel = 0;
    imageViewInfo.subresourceRange.levelCount = 1;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.layerCount = 1;

    res = vkCreateImageView(ctx.vcore.gpuDevice, &imageViewInfo, ctx.vcore.allocator, &outView);

    if (!ParseVkResult(res))
    {
        WEngine::WLog::SetConsoleError();
        WEngine::WLog::ConsoleLog("Unable to create image view.");
        return;
    }
}


Vulkan_Texture CreateTexture(VulkanContext &ctx, VulkanStatistics &stat, const WEngine::TextureInfoDDS &texInfo)
{
    Vulkan_Texture tex{};

    CreateImage(ctx, stat, texInfo, tex.image, tex.imageView, tex.imageAllocation);

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
    samplerInfo.maxLod = (float32)texInfo.mipCount;

    auto res = vkCreateSampler(ctx.vcore.gpuDevice, &samplerInfo, ctx.vcore.allocator, &tex.sampler);

    if (!ParseVkResult(res))
    {
        WEngine::WLog::SetConsoleError();
        WEngine::WLog::ConsoleLog("Unable to create sampler for texture.");
        return {};
    }

    return tex;
}

void BeginTextureUpload(VulkanContext &ctx)
{
    vkResetCommandBuffer(ctx.transferCommandBuffer, 0);
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(ctx.transferCommandBuffer, &beginInfo);
}

void QueueTexture(VulkanContext &ctx, const Vulkan_Texture& tex, const WEngine::TextureInfoDDS &texInfo)
{
    VkImageMemoryBarrier pipeBarrier{};
    pipeBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    pipeBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    pipeBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    pipeBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    pipeBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    pipeBarrier.image = tex.image;
    pipeBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    pipeBarrier.subresourceRange.baseMipLevel = 0;
    pipeBarrier.subresourceRange.levelCount = texInfo.mipCount;
    pipeBarrier.subresourceRange.baseArrayLayer = 0;
    pipeBarrier.subresourceRange.layerCount = 1;
    pipeBarrier.srcAccessMask = 0;
    pipeBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    vkCmdPipelineBarrier(ctx.transferCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
        0, 0, nullptr, 0, nullptr, 1, &pipeBarrier);

    VkBuffer staging;
    VmaAllocation stagingAlloc;

    VkDeviceSize totalSize = 0;
    uint32 mw = texInfo.width, mh = texInfo.height;
    for (uint32_t i = 0; i < texInfo.mipCount; ++i)
    {
        totalSize += BCMipSize(mw, mh, texInfo.format);
        mw = std::max(1u, mw / 2);
        mh = std::max(1u, mh / 2);
    }

    VkBufferCreateInfo bufInfo{};
    bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufInfo.size = totalSize;
    bufInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
    allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    VmaAllocationInfo allocationInfo{};
    vmaCreateBuffer(ctx.vcore.vmaAllocator, &bufInfo, &allocInfo, &staging, &stagingAlloc, &allocationInfo);

    void *mapped;
    vmaMapMemory(ctx.vcore.vmaAllocator, stagingAlloc, &mapped);
    memcpy(mapped, texInfo.data, totalSize);
    vmaUnmapMemory(ctx.vcore.vmaAllocator, stagingAlloc);

    std::vector<VkBufferImageCopy> regions;
    VkDeviceSize offset = 0;
    mw = texInfo.width;
    mh = texInfo.height;
    for (uint32_t mip = 0; mip < texInfo.mipCount; ++mip)
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
        offset += BCMipSize(mw, mh, texInfo.format);
        mw = std::max(1u, mw / 2);
        mh = std::max(1u, mh / 2);
    }

    vkCmdCopyBufferToImage(ctx.transferCommandBuffer, staging, tex.image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, regions.size(), regions.data());

    VkImageMemoryBarrier postBarrier = pipeBarrier;
    postBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    postBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    postBarrier.subresourceRange.levelCount = texInfo.mipCount;
    postBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    postBarrier.dstAccessMask = 0;

    // This must transition to bottom of pipe, not fragment. This is not on a drawing queue!!
    vkCmdPipelineBarrier(ctx.transferCommandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        0, 0, nullptr, 0, nullptr, 1, &postBarrier);

    ctx.stagingBuffers.push_back({staging, stagingAlloc});
}

void UploadTextures(VulkanContext &ctx)
{
    vkEndCommandBuffer(ctx.transferCommandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &ctx.transferCommandBuffer;

    vkQueueSubmit(ctx.queues.primaryTransferQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(ctx.queues.primaryTransferQueue);

    for (auto& staging : ctx.stagingBuffers)
    {
        vmaDestroyBuffer(ctx.vcore.vmaAllocator, staging.first, staging.second);
    }
    ctx.stagingBuffers.clear();
}


#endif
