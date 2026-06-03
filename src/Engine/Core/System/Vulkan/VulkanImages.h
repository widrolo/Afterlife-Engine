#pragma once

#include <Engine/EngineDefines.h>

#include "VulkanContext.h"
#include "VulkanStatistics.h"
#if GPU_BACKEND == GPU_VULKAN


bool SetupDepthImage(VulkanContext& ctx, VulkanStatistics& stat);

#endif
