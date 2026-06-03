#pragma once

#include <Engine/EngineDefines.h>
#if GPU_BACKEND == GPU_VULKAN

#include "VulkanContext.h"

bool SetupPipelineLayout(VulkanContext& ctx);
bool SetupDescriptorPool(VulkanContext& ctx);
bool SetupCommandPool(VulkanContext& ctx);
VkCommandBuffer CreateCommandBuffer(const VulkanContext& ctx, VkCommandPool cmdPool);
VkRenderPass CreateBasicRenderPass(VulkanContext& ctx);

#endif