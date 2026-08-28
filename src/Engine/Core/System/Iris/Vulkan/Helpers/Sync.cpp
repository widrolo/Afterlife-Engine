#if GPU_BACKEND == GPU_VULKAN

#include "Sync.h"

void PopulateSemsAndFencesSwapchain(Vulkan_RenderTargetSwapchain &rt)
{
    rt.imageAvailableSems.resize(screen.swapchainImageCount);
    rt.renderFinishedSems.resize(screen.swapchainImageCount);
    rt.endOfFrameFences.resize(screen.swapchainImageCount);

    VkSemaphoreCreateInfo semInfo{};
    semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    for (sizeT i = 0; i < screen.swapchainImageCount; i++)
        vkCreateSemaphore(vcore.gpuDevice, &semInfo, vcore.allocator, &rt.imageAvailableSems[i]);
    for (sizeT i = 0; i < screen.swapchainImageCount; i++)
        vkCreateSemaphore(vcore.gpuDevice, &semInfo, vcore.allocator, &rt.renderFinishedSems[i]);

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    for (sizeT i = 0; i < screen.swapchainImageCount; i++)
        vkCreateFence(vcore.gpuDevice, &fenceInfo, vcore.allocator, &rt.endOfFrameFences[i]);
}

void PopulateSemsAndFences(Vulkan_RenderTarget &rt)
{
    VkSemaphoreCreateInfo semInfo{};
    semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (sizeT i = 0; i < GPUSettingsVulkan::renderTargetsInFlightImages; i++)
    {
        vkCreateSemaphore(vcore.gpuDevice, &semInfo, vcore.allocator, &rt.imageAvailableSems[i]);
        vkCreateSemaphore(vcore.gpuDevice, &semInfo, vcore.allocator, &rt.renderFinishedSems[i]);
        vkCreateFence(vcore.gpuDevice, &fenceInfo, vcore.allocator, &rt.endOfFrameFences[i]);
    }
}

#endif
