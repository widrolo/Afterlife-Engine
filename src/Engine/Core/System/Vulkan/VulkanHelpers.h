#pragma once
#include <Engine/EngineDefines.h>
#if GPU_BACKEND == GPU_VULKAN

#include <vulkan/vulkan.h>
#include "VulkanContext.h"
#include "Engine/Types/Rendering/GPU/StatBufKey.h"

bool ParseVkResult(VkResult result);
VkBool32 ValidationCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*);
uint64 CalcTextureSize(uint8 bytesPerPixel, uint32 width, uint32 height);
uint64 CalcModelSize(uint8 bytesPerVertex, uint32 vertexCount);
VkFormat FindBestDepthFormat(const VulkanContext& ctx);
VkFormat FindBestSwapchainFormat(const VulkanContext& ctx);
uint64 GetSizeOfImageInBytes(WEngine::Vector2 imageSize, uint8 channelCount);
uint32 GetVulkanVersion();

WEngine::Vector2& GetFbResolution(const VulkanContext& ctx);
VkCommandBuffer& GetFbCmdBuff(const VulkanContext& ctx);
VkImage& GetFbImage(const VulkanContext& ctx);
VkImage& GetFbImage(const VulkanContext& ctx, Vulkan_RenderTarget& rt);
VkImageView& GetFbImageView(const VulkanContext& ctx);
VkSemaphore& GetFbImageAvailSem(const VulkanContext& ctx);
VkSemaphore& GetFbRenderFinishedSem(const VulkanContext& ctx);
VkFence& GetFbEndOfFrameFence(const VulkanContext& ctx);
VkDescriptorSet& GetFbDescriptorSet(const VulkanContext& ctx, Vulkan_RenderTarget& rt);
VkImageLayout& GetFbLayout(const VulkanContext& ctx);
VkImageLayout& GetFbLayout(const VulkanContext& ctx, Vulkan_RenderTarget& rt);

Vulkan_StatBuf& GetStatBuf(VulkanContext &ctx, WEngine::StatBufKey key);

void PopulatePushConstants(const VulkanContext& ctx, const Vulkan_Shader& shader, const WEngine::Mat4x4& mvp);

#endif
