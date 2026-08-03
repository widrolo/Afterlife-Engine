#pragma once

#if GPU_BACKEND == GPU_VULKAN
#include <string>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <Engine/Types/Rendering/Iris/Usage.h>

struct Vulkan_Buffer
{
    std::string debugName;
    Iris::BufferUsage usage;
    VkBuffer buffer;
    VmaAllocation alloc;
    VmaAllocationInfo allocInfo;
};

#endif