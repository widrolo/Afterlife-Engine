#pragma once

#include <Engine/EngineDefines.h>
#if GPU_BACKEND == GPU_VULKAN

#include "VulkanContext.h"
#include "VulkanStatistics.h"

void SetupSwapchainFramebuffers(VulkanContext& ctx, VulkanStatistics& stats);
bool SetupSwapchain(VulkanContext& ctx, VulkanStatistics& stats);

#endif