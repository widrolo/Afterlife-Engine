#if GPU_BACKEND == GPU_VULKAN
#include "ImGui.h"

#include "Helpers.h"
#include "Engine/Core/System/Iris/Vulkan/IrisGlobals.h"
#include "Engine/imgui/backends/imgui_impl_vulkan.h"
#include "Engine/Types/Rendering/Iris/Descriptors.h"
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

void SetupImGuiSampler()
{
    // hacky way to abuse defaults.
    Iris::SamplerDesc desc{};

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = IrisFilterToVulkan(desc.magFilter);
    samplerInfo.minFilter = IrisFilterToVulkan(desc.minFilter);
    samplerInfo.mipmapMode = IrisMipFilterToVulkan(desc.mipFilter);
    samplerInfo.addressModeU = IrisSamAddrModeToVulkan(desc.addressU);
    samplerInfo.addressModeV = IrisSamAddrModeToVulkan(desc.addressV);
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.mipLodBias = desc.mipLodBias;
    samplerInfo.anisotropyEnable = desc.anisotropyEnable;
    samplerInfo.maxAnisotropy = desc.maxAnisotropy;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = VK_LOD_CLAMP_NONE;
    samplerInfo.compareEnable = desc.compareEnable;
    samplerInfo.compareOp = IrisCompareOpToVulkan(desc.compareOp);
    samplerInfo.borderColor = IrisBorderColorToVulkan(desc.borderColor);

    auto res = vkCreateSampler(vcore.gpuDevice, &samplerInfo, vcore.allocator, &imGuiSampler);

    if (!ParseVkResult(res))
    {
        WEngine::WLog::SetConsoleError();
        WEngine::WLog::ConsoleLog("Unable to create ImGui sampler.");
    }
}

#endif
