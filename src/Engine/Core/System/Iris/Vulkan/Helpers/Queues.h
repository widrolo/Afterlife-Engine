#pragma once
#if GPU_BACKEND == GPU_VULKAN

#include "Engine/WTL/vector.h"
#include <vulkan/vulkan.h>

wtl::vector<VkDeviceQueueCreateInfo> FindDeviceQueues();
void SetupDeviceQueues();

#endif
