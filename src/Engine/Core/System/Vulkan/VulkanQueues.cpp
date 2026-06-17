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


        float32* queuePriorities = wNewArr(float32, properties.queueCount);

        // ooga booga first has high priority
        for (int j = 0; j < properties.queueCount; j++)
            queuePriorities[j] = 0.0f;
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
            ctx.queues.primaryDrawQueueFamilyIndex = count;
            break;
        }
        count++;
    }
    count = 0;
    for (auto& familiy : ctx.queues.queueFamilies)
    {
        if (familiy.purpose & (uint8)QueuePurpose::Transfer)
        {
            ctx.queues.primaryTransferQueueFamilyIndex = count;

            // basically, were being racist towards the draw family
            if (count != ctx.queues.primaryDrawQueueFamilyIndex)
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
        for (int j = 0; j < ctx.queues.queueFamilies[i].queues.size(); j++)
        {
            vkGetDeviceQueue(ctx.vcore.gpuDevice, i, j, &ctx.queues.queueFamilies[i].queues[j]);


            if (ctx.queues.queueFamilies[i].purpose & (uint8)QueuePurpose::Drawing)
            {
                ctx.queues.primaryDrawQueue = ctx.queues.queueFamilies[i].queues[j];
            }

            if (ctx.queues.queueFamilies[i].purpose & (uint8)QueuePurpose::Transfer)
            {
                ctx.queues.primaryTransferQueue = ctx.queues.queueFamilies[i].queues[j];
            }
        }

    }
}

#endif