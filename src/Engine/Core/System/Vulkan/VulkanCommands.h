#pragma once
#include <Engine/EngineDefines.h>
#if GPU_BACKEND == GPU_VULKAN
#include <vulkan/vulkan.h>

#include "VulkanContext.h"
#include "VulkanStatistics.h"
#include "Engine/Types/Rendering/GPU/Framebuffer.h"

bool SetupDrawCommandPool(VulkanContext& ctx);
VkCommandBuffer CreateCommandBuffer(const VulkanContext& ctx);
bool SetupDisplayRenderTarget(VulkanContext& ctx, VulkanStatistics& stat);
Vulkan_RenderTarget CreateRenderTarget(VulkanContext& ctx, VulkanStatistics& stat, const WEngine::Vector2& resolution);
void PopulateSemsAndFences(VulkanContext& ctx, Vulkan_RenderTarget& rt);

#endif