#pragma once
#if GPU_BACKEND == GPU_VULKAN

#include "Engine/WTL/vector.h"
#include <vulkan/vulkan.h>

#include "Engine/Types/Rendering/Iris/FixedFunction.h"

wtl::vector<VkDeviceQueueCreateInfo> FindDeviceQueues();
void SetupDeviceQueues();
VkQueue QueueFor(Iris::QueueType type);
uint32 QueueFamilyFor(Iris::QueueType type);

#endif
