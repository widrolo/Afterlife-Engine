#pragma once

#if GPU_BACKEND == GPU_VULKAN
#include "VulkanContext.h"
#include "VulkanStatistics.h"
#include "Engine/Types/Rendering/TextureInfo.h"


bool SetupDepthImage(VulkanContext& ctx, VulkanStatistics& stat);


void CreateImage(VulkanContext& ctx, VulkanStatistics& stat, const WEngine::TextureInfoDDS &tex, VkImage& outImg,
                 VkImageView& outView, VmaAllocation& outAlloc);
void CreateImageRenderTarget(VulkanContext& ctx, VulkanStatistics &stat, const WEngine::Vector2& size,
    VkImage& outImg, VkImageView& outView, VmaAllocation& outAlloc);
Vulkan_Texture CreateTexture(VulkanContext& ctx, VulkanStatistics& stat, const WEngine::TextureInfoDDS& texInfo);
void BeginTextureUpload(VulkanContext& ctx);
void QueueTexture(VulkanContext& ctx, const Vulkan_Texture& tex, const WEngine::TextureInfoDDS &texInfo);
void UploadTextures(VulkanContext& ctx);

#endif
