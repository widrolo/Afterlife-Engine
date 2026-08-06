#if GPU_BACKEND == GPU_VULKAN

#include "CmdBuffs.h"

#include "Helpers.h"
#include "Queues.h"
#include "../IrisGlobals.h"
#include "Engine/Types/Rendering/Iris/FixedFunction.h"
#include "Engine/Util/Log.h"

bool SetupCommandBuffers()
{
    framePools.resize(screen.swapchainImageCount);
    for (Vulkan_FramePools& slot : framePools)
    {
        for (uint32 q = 0; q < 3; q++)
        {
            VkCommandPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
            poolInfo.queueFamilyIndex = QueueFamilyFor((Iris::QueueType)q);

            auto res = vkCreateCommandPool(vcore.gpuDevice, &poolInfo, vcore.allocator, &slot.pool[q]);

            if (!ParseVkResult(res))
            {
                WEngine::WLog::SetConsoleError();
                WEngine::WLog::ConsoleLog("Unable to create command pool");
                return false;
            }
        }
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;   // so the first NewFrame doesn't block forever

        auto res = vkCreateFence(vcore.gpuDevice, &fenceInfo, vcore.allocator, &slot.fence);

        if (!ParseVkResult(res))
        {
            WEngine::WLog::SetConsoleError();
            WEngine::WLog::ConsoleLog("Unable to create command pool fence");
            return false;
        }
    }
    return true;
}


#endif
