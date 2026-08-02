#if GPU_BACKEND == GPU_VULKAN
#include "Queues.h"

#include "Engine/Core/System/Iris/Vulkan/IrisGlobals.h"

wtl::vector<VkDeviceQueueCreateInfo> FindDeviceQueues()
{
    vkGetPhysicalDeviceQueueFamilyProperties2(vcore.gpuPhysicalDevice, &queues.queueFamilyCount, nullptr);
    wtl::vector<VkQueueFamilyProperties2> families(queues.queueFamilyCount);
    for (auto& family : families)
        family.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
    vkGetPhysicalDeviceQueueFamilyProperties2(vcore.gpuPhysicalDevice, &queues.queueFamilyCount, families.data());

    queues.queueFamilies.resize(queues.queueFamilyCount);
    wtl::vector<VkDeviceQueueCreateInfo> infos(queues.queueFamilyCount);

    for (int i = 0; i < queues.queueFamilyCount; i++)
    {
        const auto& properties = families[i].queueFamilyProperties;
        queues.queueFamilies[i].familyIndex = i;
        queues.queueFamilies[i].queues.resize(properties.queueCount);
        queues.queueFamilies[i].purpose = properties.queueFlags;


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
    for (auto& familiy : queues.queueFamilies)
    {
        if (familiy.purpose & (uint8)QueuePurpose::Drawing)
        {
            queues.primaryDrawQueueFamilyIndex = count;
            break;
        }
        count++;
    }
    count = 0;
    for (auto& familiy : queues.queueFamilies)
    {
        if (familiy.purpose & (uint8)QueuePurpose::Transfer)
        {
            queues.primaryTransferQueueFamilyIndex = count;

            // basically, were being racist towards the draw family
            if (count != queues.primaryDrawQueueFamilyIndex)
                break;
        }
        count++;
    }
    return infos;
}

void SetupDeviceQueues()
{
    for (int i = 0; i < queues.queueFamilyCount; i++)
    {
        for (int j = 0; j < queues.queueFamilies[i].queues.size(); j++)
        {
            vkGetDeviceQueue(vcore.gpuDevice, i, j, &queues.queueFamilies[i].queues[j]);
            if (i == queues.primaryDrawQueueFamilyIndex && j == 0)
                queues.primaryDrawQueue = queues.queueFamilies[i].queues[j];
            if (i == queues.primaryTransferQueueFamilyIndex && j == 0)
                queues.primaryTransferQueue = queues.queueFamilies[i].queues[j];
        }
    }
}

#endif
