#pragma once

#include <Engine/EngineDefines.h>
//#if GPU_BACKEND == GPU_VULKAN
#ifdef ehbwfaj

#include "Engine/WTL/vector.h"
#include <vulkan/vulkan.h>
#include "VulkanContext.h"

wtl::vector<VkDeviceQueueCreateInfo> FindDeviceQueues(VulkanContext& ctx);
void SetupDeviceQueues(VulkanContext& ctx);

#endif
