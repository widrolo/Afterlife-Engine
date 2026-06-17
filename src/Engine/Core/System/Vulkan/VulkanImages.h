#pragma once

#if GPU_BACKEND == GPU_VULKAN
#include "VulkanContext.h"
#include "VulkanStatistics.h"
#include "Engine/Types/Rendering/TextureInfo.h"


bool SetupDepthImage(VulkanContext& ctx, VulkanStatistics& stat);
bool SetupTransferCommandBuffer(VulkanContext& ctx);

Vulkan_Texture CreateTexture(VulkanContext& ctx, VulkanStatistics& stat, const WEngine::TextureInfo& texInfo);
void BeginTextureUpload(VulkanContext& ctx);
void QueueTexture(VulkanContext& ctx, const Vulkan_Texture& tex, const WEngine::TextureInfo &texInfo);
void UploadTextures(VulkanContext& ctx);

#endif
