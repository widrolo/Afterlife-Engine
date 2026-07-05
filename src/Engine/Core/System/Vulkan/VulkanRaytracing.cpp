#include <Engine/EngineDefines.h>
#if GPU_BACKEND == GPU_VULKAN

#include "VulkanRaytracing.h"

#include "VulkanBuffers.h"
#include "Engine/Types/Rendering/VertextData.h"
#include "Engine/Util/Log.h"

void WEngine_vkGetAccelerationStructureBuildSizesKHR(VulkanContext& ctx,
    VkAccelerationStructureBuildTypeKHR buildType,const VkAccelerationStructureBuildGeometryInfoKHR *pBuildInfo,
    const uint32_t *pMaxPrimitiveCounts,VkAccelerationStructureBuildSizesInfoKHR *pSizeInfo)
{
    static auto vkGetAccelerationStructureBuildSizesKHR = (PFN_vkGetAccelerationStructureBuildSizesKHR)vkGetDeviceProcAddr(
        ctx.vcore.gpuDevice, "vkGetAccelerationStructureBuildSizesKHR");

    if (!vkGetAccelerationStructureBuildSizesKHR)
    {
        WEngine::WLog::SetConsoleError();
        WEngine::WLog::ConsoleLog("Failed to load vkGetAccelerationStructureBuildSizesKHR");
        return;
    }

    vkGetAccelerationStructureBuildSizesKHR(ctx.vcore.gpuDevice, buildType, pBuildInfo, pMaxPrimitiveCounts, pSizeInfo);
}

VkResult WEngine_vkCreateAccelerationStructureKHR(VulkanContext& ctx, const VkAccelerationStructureCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkAccelerationStructureKHR* pAccelerationStructure)
{
    static auto vkCreateAccelerationStructureKHR = (PFN_vkCreateAccelerationStructureKHR)vkGetDeviceProcAddr(
        ctx.vcore.gpuDevice, "vkCreateAccelerationStructureKHR");

    if (!vkCreateAccelerationStructureKHR)
    {
        WEngine::WLog::SetConsoleError();
        WEngine::WLog::ConsoleLog("Failed to load vkCreateAccelerationStructureKHR");
        return VK_ERROR_UNKNOWN;
    }

    return vkCreateAccelerationStructureKHR(ctx.vcore.gpuDevice, pCreateInfo, pAllocator, pAccelerationStructure);
}

void WEngine_vkCmdBuildAccelerationStructuresKHR(VulkanContext& ctx, uint32_t infoCount,
                                                 const VkAccelerationStructureBuildGeometryInfoKHR* pInfos,
                                                 const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos)
{
    static auto vkCmdBuildAccelerationStructuresKHR = (PFN_vkCmdBuildAccelerationStructuresKHR)vkGetDeviceProcAddr(
        ctx.vcore.gpuDevice, "vkCmdBuildAccelerationStructuresKHR");

    if (!vkCmdBuildAccelerationStructuresKHR)
    {
        WEngine::WLog::SetConsoleError();
        WEngine::WLog::ConsoleLog("Failed to load vkCmdBuildAccelerationStructuresKHR");
        return;
    }

    vkCmdBuildAccelerationStructuresKHR(ctx.transferCommandBuffer, infoCount, pInfos, ppBuildRangeInfos);
}

bool AddModelToBLAS(VulkanContext &ctx, VulkanStatistics& stat, Vulkan_Model &model)
{
    VkBufferDeviceAddressInfo vertBufDevInfo{};
    vertBufDevInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    vertBufDevInfo.buffer = model.vertexBuffer;
    VkBufferDeviceAddressInfo indBufDevInfo{};
    indBufDevInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    indBufDevInfo.buffer = model.indexBuffer;

    VkDeviceAddress vertBufDevAddr = vkGetBufferDeviceAddress(ctx.vcore.gpuDevice, &vertBufDevInfo);
    VkDeviceAddress indBufDevAddr = vkGetBufferDeviceAddress(ctx.vcore.gpuDevice, &indBufDevInfo);

    VkAccelerationStructureGeometryTrianglesDataKHR tris{};
    tris.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
    tris.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
    tris.vertexData.deviceAddress = vertBufDevAddr;
    tris.vertexStride = sizeof(WEngine::VertexData);
    tris.maxVertex = model.vertexCount - 1;
    tris.indexType = VK_INDEX_TYPE_UINT32;
    tris.indexData.deviceAddress = indBufDevAddr;

    VkAccelerationStructureGeometryKHR geo{};
    geo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
    geo.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
    geo.geometry.triangles = tris;

    VkAccelerationStructureBuildGeometryInfoKHR buildInfo{};
    buildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    buildInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    buildInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    buildInfo.geometryCount = 1;
    buildInfo.pGeometries = &geo;

    uint32_t primitiveCount = model.indexCount / 3;

    VkAccelerationStructureBuildSizesInfoKHR sizeInfo{};
    sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
    WEngine_vkGetAccelerationStructureBuildSizesKHR(ctx, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &buildInfo,
        &primitiveCount, &sizeInfo);

    auto asBuf =  CreateAccelerationStructureBuffer(ctx, stat, sizeInfo.accelerationStructureSize);
    auto scrBuf =  CreateAccelerationScratchBuffer(ctx, stat, sizeInfo.buildScratchSize);
    model.asBuffer = asBuf.first;
    model.asAllocation = asBuf.second;
    model.scrBuffer = scrBuf.first;
    model.scrAllocation = scrBuf.second;

    VkAccelerationStructureCreateInfoKHR asInfo{};
    asInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
    asInfo.buffer = model.asBuffer;
    asInfo.size = sizeInfo.accelerationStructureSize;
    asInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;

    WEngine_vkCreateAccelerationStructureKHR(ctx, &asInfo, ctx.vcore.allocator, &model.blas);

    VkBufferDeviceAddressInfo scrBufDevInfo{};
    scrBufDevInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    scrBufDevInfo.buffer = model.scrBuffer;
    VkDeviceAddress scrBufDevAddr = vkGetBufferDeviceAddress(ctx.vcore.gpuDevice, &scrBufDevInfo);

    buildInfo.dstAccelerationStructure = model.blas;
    buildInfo.scratchData.deviceAddress = scrBufDevAddr;

    VkAccelerationStructureBuildRangeInfoKHR buildRangeInfo{};
    buildRangeInfo.primitiveCount = primitiveCount;
    buildRangeInfo.primitiveOffset = 0;
    buildRangeInfo.firstVertex = 0;
    buildRangeInfo.transformOffset = 0;

    VkAccelerationStructureBuildRangeInfoKHR* pRange = &buildRangeInfo;

    WEngine_vkCmdBuildAccelerationStructuresKHR(ctx, 1, &buildInfo, &pRange);

    return true;
}


#endif

