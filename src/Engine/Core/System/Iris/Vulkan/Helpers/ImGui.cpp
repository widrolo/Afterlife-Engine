#if GPU_BACKEND == GPU_VULKAN
#include "ImGui.h"

#include "Helpers.h"
#include "Engine/Core/System/Iris/Vulkan/IrisGlobals.h"
#include "Engine/imgui/backends/imgui_impl_vulkan.h"
#include "Engine/Util/Log.h"

VkDescriptorPool SetupImGuiDescriptorPool()
{
    wtl::vector<VkDescriptorPoolSize> poolSizes =
    {
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE},
        { VK_DESCRIPTOR_TYPE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE },
    };
    VkDescriptorPoolCreateInfo descInfo{};
    descInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    descInfo.maxSets = 0;
    for (VkDescriptorPoolSize& pool_size : poolSizes)
        descInfo.maxSets += pool_size.descriptorCount;
    descInfo.poolSizeCount = poolSizes.size();
    descInfo.pPoolSizes = poolSizes.data();

    VkDescriptorPool pool;
    auto res = vkCreateDescriptorPool(vcore.gpuDevice, &descInfo, vcore.allocator, &pool);

    if (!ParseVkResult(res))
    {
        WEngine::WLog::SetConsoleError();
        WEngine::WLog::ConsoleLog("Unable to create descriptor pool!");
        return VK_NULL_HANDLE;
    }
    return pool;
}

#endif
