#pragma once
#if GPU_BACKEND == GPU_VULKAN

#include <vulkan/vulkan.h>

VkPresentModeKHR FindBestPresentMode();
bool SetupSwapchain();
bool SetupDepthImage();

#endif
