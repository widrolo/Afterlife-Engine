#pragma once

#if GPU_BACKEND == GPU_VULKAN
#include <string>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <Engine/Types/Rendering/Iris/Usage.h>

#include "Engine/Types/Rendering/Iris/Handles.h"
#include "Engine/WTL/vector.h"

struct Vulkan_Buffer
{
    std::string debugName;
    Iris::BufferUsage usage;
    VkBuffer buffer;
    VmaAllocation alloc;
    VmaAllocationInfo allocInfo;
};

struct Vulkan_Texture
{
    std::string debugName;
    VkImage image;
    VkImageView imageView;
    VmaAllocation alloc;
    VmaAllocationInfo allocInfo;
    uint32 width;
    uint32 height;
    uint8 mipCount;
    VkFormat format;

    VkDescriptorSet imGuiDescriptorSet;
};

struct Vulkan_Sampler
{
    std::string debugName;
    VkSampler sampler;
};

struct Vulkan_Shader
{
    std::string debugName;
    VkShaderModule shader;
    VkShaderStageFlags stage;
};

struct Vulkan_ResourceTableLayout
{
    std::string debugName;
    VkDescriptorSetLayout layout;
    wtl::vector<VkDescriptorSetLayoutBinding> bindings;
    wtl::vector<VkDescriptorPoolSize> poolSizes;
};

struct Vulkan_ResourceTable
{
    std::string debugName;
    Iris::ResourceTableLayoutHandle layoutHandle;
    VkDescriptorPool pool;
    VkDescriptorSet set;
};

struct Vulkan_CmdBuff
{
    wtl::vector<VkCommandBuffer> commandBuffers;
    VkQueue queue = VK_NULL_HANDLE;
};

struct Vulkan_CopyBuff
{
    VkCommandBuffer commandBuffer;
    VkCommandPool pool;
    VkFence fence;
    VkQueue queue = VK_NULL_HANDLE;
};

struct Vulkan_FramePools
{
    VkCommandPool pool[3] = {};
    VkFence fence = VK_NULL_HANDLE;
};

struct Vulkan_Pipeline
{
    std::string debugName;
    VkPipeline pipeline;
    VkPipelineLayout layout;
    VkShaderStageFlags pushStageFlags;
};


#endif