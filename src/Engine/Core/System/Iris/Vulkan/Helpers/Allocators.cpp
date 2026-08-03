#if GPU_BACKEND == GPU_VULKAN

#include "Allocators.h"

#include "Helpers.h"
#include "Engine/Core/System/GPUSettings.h"
#include "Engine/Core/System/Memory.h"
#include "Engine/Core/System/Iris/Vulkan/IrisGlobals.h"

void* VulkanAllocate(void*, size_t size, size_t alignment, VkSystemAllocationScope scope)
{
    return WAllocator::AllocateAligned(size, alignment);
}
void* VulkanReallocate(void*, void* ptr, size_t newSize, size_t alignment, VkSystemAllocationScope scope)
{
    return WAllocator::ReallocateAligned(ptr, newSize, alignment);
}
void VulkanFree(void*, void* ptr)
{
    WAllocator::Free(ptr);
}
void VulkanInternalAllocation(void*, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope scope)
{
    WAllocator::ReportExternalAllocation(size);
}
void VulkanInternalFree(void*, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope scope)
{
    WAllocator::ReportExternalFree(size);
}

void SetupAllocator()
{
    if constexpr (!GPUSettingsVulkan::useWAllocator)
        return;

    vcore.allocatorInternal.pfnAllocation = VulkanAllocate;
    vcore.allocatorInternal.pfnReallocation = VulkanReallocate;
    vcore.allocatorInternal.pfnFree = VulkanFree;
    vcore.allocatorInternal.pfnInternalAllocation = VulkanInternalAllocation;
    vcore.allocatorInternal.pfnInternalFree = VulkanInternalFree;

    vcore.allocator = &vcore.allocatorInternal;
}

bool SetupVmaAllocator()
{
    VmaVulkanFunctions vulkanFunctions = {};
    vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
    vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

    VmaAllocatorCreateInfo allocatorCreateInfo = {};
    allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
    allocatorCreateInfo.vulkanApiVersion = GetVulkanVersion();
    allocatorCreateInfo.physicalDevice = vcore.gpuPhysicalDevice;
    allocatorCreateInfo.device = vcore.gpuDevice;
    allocatorCreateInfo.instance = vcore.instance;
    allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;

    auto res = vmaCreateAllocator(&allocatorCreateInfo, &vcore.vmaAllocator);

    return ParseVkResult(res);
}

#endif
