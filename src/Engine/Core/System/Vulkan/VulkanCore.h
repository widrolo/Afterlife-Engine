#pragma once

#include <Engine/EngineDefines.h>
#if GPU_BACKEND == GPU_VULKAN
#include <vulkan/vulkan.h>
#include "Engine/WTL/vector.h"
#include "VulkanContext.h"


void* VulkanAllocate(void*, size_t size, size_t alignment, VkSystemAllocationScope scope);
void* VulkanReallocate(void*, void* ptr, size_t newSize, size_t alignment, VkSystemAllocationScope scope);
void VulkanFree(void*, void* ptr);
void VulkanInternalAllocation(void*, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope scope);
void VulkanInternalFree(void*, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope scope);

void SetupAllocator(VulkanContext& ctx);
void SetupVmaAllocator(VulkanContext& ctx);
bool SetupValidation(VulkanContext& ctx);
wtl::vector<std::string> GetExtensionsToLoad(VulkanContext& ctx);
bool SetupVkInstance(VulkanContext& ctx);
VkPhysicalDeviceRayQueryFeaturesKHR& GetVkPhysicalDeviceRayQueryFeatures();
bool SetupGraphicsDevice(VulkanContext& ctx);

#endif