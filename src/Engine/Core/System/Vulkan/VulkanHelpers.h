#pragma once
#include <Engine/EngineDefines.h>
#if GPU_BACKEND == GPU_VULKAN

#include <vulkan/vulkan.h>
#include "VulkanContext.h"

bool ParseVkResult(VkResult result);
VkBool32 ValidationCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*);
uint64 CalcTextureSize(uint8 bytesPerPixel, uint32 width, uint32 height);
uint64 CalcModelSize(uint8 bytesPerVertex, uint32 vertexCount);
VkFormat FindBestDepthFormat(const VulkanContext& ctx);
VkFormat FindBestSwapchainFormat(const VulkanContext& ctx);
uint64 GetSizeOfImageInBytes(WEngine::Vector2 imageSize, uint8 channelCount);
uint32 GetVulkanVersion();

VkCommandBuffer& GetCmdBuff(const VulkanContext& ctx);
VkImage& GetFbImage(const VulkanContext& ctx);
VkImageView& GetFbImageView(const VulkanContext& ctx);

#endif
