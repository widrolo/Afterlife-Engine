#pragma once
#include <Engine/EngineDefines.h>
#if GPU_BACKEND == GPU_VULKAN

#include <vulkan/vulkan.h>
#include "VulkanContext.h"
#include "Engine/Core/System/GPUSettings.h"
#include "Engine/Types/Rendering/GPU/Framebuffer.h"
#include "Engine/Types/Rendering/GPU/StatBufKey.h"

bool ParseVkResult(VkResult result);
VkBool32 ValidationCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*);
void PerformInvalidHandleAction();
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
VkImageView& GetFbImageView(const VulkanContext& ctx, Vulkan_RenderTarget& rt);
VkSemaphore& GetFbImageAvailSem(const VulkanContext& ctx);
VkSemaphore& GetFbRenderFinishedSem(const VulkanContext& ctx);
VkFence& GetFbEndOfFrameFence(const VulkanContext& ctx);
VkDescriptorSet& GetFbDescriptorSet(const VulkanContext& ctx, Vulkan_RenderTarget& rt);
VkImageLayout& GetFbLayout(const VulkanContext& ctx);
VkImageLayout& GetFbLayout(const VulkanContext& ctx, Vulkan_RenderTarget& rt);

Vulkan_StatBuf& GetStatBuf(VulkanContext &ctx, WEngine::StatBufKey key);
Vulkan_Material& GetLoadedMaterial(VulkanContext& ctx, WEngine::Material material);
Vulkan_Shader& GetLoadedShader(VulkanContext& ctx, WEngine::Shader shader);
Vulkan_Model& GetLoadedModel(VulkanContext& ctx, WEngine::Model model);
Vulkan_RenderTarget& GetLoadedRenderTarget(VulkanContext& ctx, WEngine::Framebuffer fb);

bool AddExtensionIfAvailable(VulkanContext& ctx, wtl::vector<const char*>& destination, std::string extensionName);

void PopulatePushConstants(const VulkanContext& ctx, const Vulkan_Shader& shader, const WEngine::Mat4x4& mvp);

#endif
