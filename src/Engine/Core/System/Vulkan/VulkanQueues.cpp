#include <Engine/EngineDefines.h>
#if GPU_BACKEND == GPU_VULKAN

#include "VulkanQueues.h"

wtl::vector<VkDeviceQueueCreateInfo> FindDeviceQueues(VulkanContext& ctx)
{
    vkGetPhysicalDeviceQueueFamilyProperties2(ctx.vcore.gpuPhysicalDevice, &ctx.queues.queueFamilyCount, nullptr);
    wtl::vector<VkQueueFamilyProperties2> families(ctx.queues.queueFamilyCount);
    for (auto& family : families)
        family.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
    vkGetPhysicalDeviceQueueFamilyProperties2(ctx.vcore.gpuPhysicalDevice, &ctx.queues.queueFamilyCount, families.data());

    ctx.queues.queueFamilies.resize(ctx.queues.queueFamilyCount);
    wtl::vector<VkDeviceQueueCreateInfo> infos(ctx.queues.queueFamilyCount);

    for (int i = 0; i < ctx.queues.queueFamilyCount; i++)
    {
        const auto& properties = families[i].queueFamilyProperties;
        ctx.queues.queueFamilies[i].familyIndex = i;
        ctx.queues.queueFamilies[i].queues.resize(properties.queueCount);
        ctx.queues.queueFamilies[i].purpose = properties.queueFlags;

        // yes its leaking, but its so little that it wouldnt even matter on a snes.
        float32* queuePriorities = (float32*)WAllocator::Allocate(properties.queueCount * sizeof(float32));
        // ooga booga first has high priority
        for (int i = 0; i < properties.queueCount; i++)
            queuePriorities[i] = 0.0f;
        queuePriorities[0] = 1.0f;

        VkDeviceQueueCreateInfo queueInfo{};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex = i;
        queueInfo.queueCount = properties.queueCount;
        queueInfo.pQueuePriorities = queuePriorities;
        infos[i] = queueInfo;
    }

    uint32 count = 0; // piss and shit code
    for (auto& familiy : ctx.queues.queueFamilies)
    {
        if (familiy.purpose & (uint8)QueuePurpose::Drawing)
        {
            ctx.queues.primaryDrawQueue = familiy.queues[0];
            ctx.queues.primaryDrawQueueFamilyIndex = count;
            break;
        }
        count++;
    }

    return infos;
}

void SetupDeviceQueues(VulkanContext& ctx)
{
    for (int i = 0; i < ctx.queues.queueFamilyCount; i++)
    {
        if (ctx.queues.queueFamilies[i].purpose & (uint8)QueuePurpose::Drawing)
        {
            vkGetDeviceQueue(ctx.vcore.gpuDevice, i, 0, &ctx.queues.queueFamilies[i].queues[0]);
            ctx.queues.primaryDrawQueue = ctx.queues.queueFamilies[i].queues[0];
            break;
        }
    }
}

#endif