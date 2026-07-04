#include <Engine/EngineDefines.h>
#if GPU_BACKEND == GPU_VULKAN

#include "VulkanRaytracing.h"

#include "Engine/Types/Rendering/VertextData.h"
#include "Engine/Util/Log.h"

bool AddModelToBLAS(VulkanContext &ctx, Vulkan_Model &model)
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

    VkAccelerationStructureBuildSizesInfoKHR sizeInfo{};
    vkGetAccelerationStructureBuildSizesKHR(ctx.vcore.gpuDevice, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
        &buildInfo, &model.vertexCount, &sizeInfo);

    // just to test if it works:
    WEngine::WLog::ConsoleLog(std::format("Build Size Info: structure size = {}; update = {}; build = {}",
        sizeInfo.accelerationStructureSize, sizeInfo.updateScratchSize, sizeInfo.buildScratchSize));

    return true;
}


#endif

