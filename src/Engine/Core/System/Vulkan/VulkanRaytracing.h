#pragma once

#include <Engine/EngineDefines.h>
#if GPU_BACKEND == GPU_VULKAN

#include "VulkanContext.h"
#include "VulkanStatistics.h"

void WEngine_vkGetAccelerationStructureBuildSizesKHR(VulkanContext& ctx,
    VkAccelerationStructureBuildTypeKHR buildType,const VkAccelerationStructureBuildGeometryInfoKHR *pBuildInfo,
    const uint32_t *pMaxPrimitiveCounts,VkAccelerationStructureBuildSizesInfoKHR *pSizeInfo);

VkResult WEngine_vkCreateAccelerationStructureKHR(VulkanContext& ctx, const VkAccelerationStructureCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkAccelerationStructureKHR* pAccelerationStructure);

VkResult vkCreateAccelerationStructureKHR(VulkanContext& ctx, const VkAccelerationStructureCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkAccelerationStructureKHR* pAccelerationStructure);

void WEngine_vkCmdBuildAccelerationStructuresKHR(VulkanContext& ctx, uint32_t infoCount,
    const VkAccelerationStructureBuildGeometryInfoKHR* pInfos,
    const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos);

bool AddModelToBLAS(VulkanContext& ctx, VulkanStatistics& stat, Vulkan_Model& model);

#endif
