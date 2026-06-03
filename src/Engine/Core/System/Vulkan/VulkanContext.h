#pragma once
#include <Engine/EngineDefines.h>
#if GPU_BACKEND == GPU_VULKAN

#include "VulkanTypes.h"

struct VulkanContext
{
    Vulkan_Core vcore{};
    Vulkan_Screen screen{};
    Vulkan_Queues queues{};
    Vulkan_StatBuf statBuf{};

    VkPipelineLayout pipelineLayout{};
    VkDescriptorPool descriptorPool{};
    VkCommandPool commandPool{};

    wtl::vector<VkCommandBuffer> cmdBufs{};
    VkRenderPass renderPass{};

    wtl::vector<Vulkan_Shader> loadedShaders{};
    std::unordered_map<std::string, WEngine::Shader> loadedShadersHandles{};
    wtl::vector<Vulkan_Model> loadedModels{};
    std::unordered_map<std::string, WEngine::Model> loadedModelHandles{};

    uint32 currentBoundShader = 999999999;
    std::vector<BufferCollection> bufferGraveyard{};
};

#endif
