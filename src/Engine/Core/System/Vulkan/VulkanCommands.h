#pragma once
#include <Engine/EngineDefines.h>
#if GPU_BACKEND == GPU_VULKAN
#include <vulkan/vulkan.h>

#include "VulkanContext.h"
#include "VulkanStatistics.h"

bool SetupDrawCommandPool(VulkanContext& ctx);
VkCommandBuffer CreateCommandBuffer(const VulkanContext& ctx);
bool SetupDisplayRenderTarget(VulkanContext& ctx, VulkanStatistics& stat);

#endif