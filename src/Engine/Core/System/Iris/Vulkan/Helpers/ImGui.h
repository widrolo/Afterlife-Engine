#pragma once
#if GPU_BACKEND == GPU_VULKAN

#include <vulkan/vulkan.h>

VkDescriptorPool SetupImGuiDescriptorPool();

#endif
