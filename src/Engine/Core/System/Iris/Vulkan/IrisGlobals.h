#pragma once
#if GPU_BACKEND == GPU_VULKAN

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "Engine/Types/CommonTypes.h"
#include "Engine/Util/BitwiseMacros.h"
#include "Engine/Math/Vector.h"
#include "Engine/WTL/vector.h"
#include "Helpers/Types.h"

//using namespace Iris;

struct VulkanCore
{
    VkDebugUtilsMessengerEXT validationMessenger = VK_NULL_HANDLE;
    VkAllocationCallbacks* allocator = nullptr;
    VkAllocationCallbacks allocatorInternal{};
    VmaAllocator vmaAllocator = VK_NULL_HANDLE;

    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice gpuPhysicalDevice = VK_NULL_HANDLE;
    VkDevice gpuDevice = VK_NULL_HANDLE;
};

enum class QueuePurpose : uint8
{
    Drawing     = BIT(0),
    Compute     = BIT(1),
    Transfer    = BIT(2),
};

struct QueueFamily
{
    uint8 familyIndex;
    uint8 purpose;
    wtl::vector<VkQueue> queues;
};

struct Vulkan_Queues
{
    uint32 queueFamilyCount = 0;
    wtl::vector<QueueFamily> queueFamilies;
    uint32 primaryDrawQueueFamilyIndex = 0;
    VkQueue primaryDrawQueue = VK_NULL_HANDLE;

    // if no dedicated transfer queue was found, then this will be the same as the graphics queue
    uint32 primaryTransferQueueFamilyIndex = 0;
    VkQueue primaryTransferQueue = VK_NULL_HANDLE;
};

struct Vulkan_Screen
{
    VkSurfaceKHR screen = VK_NULL_HANDLE;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    uint32 swapchainImageCount = 0;
    uint32 swapchainCurrentImage = 0;

    uint32 currentFrame = 0;

    VkImage depthImage;
    VkImageView depthImageView;
    VmaAllocation depthAllocation;
};

struct Vulkan_RenderTarget
{
    WEngine::Vector2 resolution;
    wtl::vector<VkImage> targetImages;
    wtl::vector<VkImageView> targetImageViews;
    wtl::vector<VkSampler> targetSampler;
    wtl::vector<VkImage> depthImages;
    wtl::vector<VkImageView> depthImageViews;
    wtl::vector<VkSampler> depthSampler;
    wtl::vector<VmaAllocation> targetImageAlloc;
    wtl::vector<VmaAllocation> depthImageAlloc;
    wtl::vector<VkSemaphore> imageAvailableSems;
    wtl::vector<VkSemaphore> renderFinishedSems;
    wtl::vector<VkFence> endOfFrameFences;
    wtl::vector<VkCommandBuffer> cmdBuffs;
    wtl::vector<VkDescriptorSet> descSets;
    wtl::vector<VkDescriptorSet> depthDescSets;
    wtl::vector<VkImageLayout> currentLayouts;
    wtl::vector<VkImageLayout> currentDepthLayouts;
    bool hasDepth;
};

struct VulkanStatistics
{
    uint32 drawCallsThisFrame = 0;
    uint32 drawCallsLastFrame = 0;
    uint64 vramUsage = 0;
};

using BufferCollection = wtl::vector<std::pair<VkBuffer, VmaAllocation>>;

struct IrisContext
{
    bool firstFrame = true;
};

inline IrisContext irisCtx{};
inline VulkanCore vcore{};
inline Vulkan_Queues queues{};
inline Vulkan_Screen screen{};
inline Vulkan_RenderTarget displayTarget{};
inline VulkanStatistics stats{};
inline wtl::vector<BufferCollection> bufferGraveyard{};
inline wtl::vector<Vulkan_Buffer> loadedBuffers{};
inline wtl::vector<Vulkan_Texture> loadedTextures{};
inline wtl::vector<Vulkan_Sampler> loadedSamplers{};
inline wtl::vector<Vulkan_Shader> loadedShaders{};
inline wtl::vector<Vulkan_ResourceTableLayout> loadedResourceTableLayouts{};
inline wtl::vector<Vulkan_ResourceTable> loadedResourceTables{};

#endif