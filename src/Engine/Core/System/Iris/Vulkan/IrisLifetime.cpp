#if GPU_BACKEND == GPU_VULKAN

#include <Engine/Core/System/Iris.h>

#include "Helpers/Helpers.h"
#include "IrisGlobals.h"

namespace Iris
{
    uint32 GetCurrentFrameIndex()
    {
        return screen.currentFrame;
    }

    uint32 GetFramesInFlight()
    {
        return screen.swapchainImageCount;
    }

    bool IsFirstFrame()
    {
        return irisCtx.firstFrame;
    }

    void BeginFrame()
    {
        commandBufferFrameIndex++;
        const uint32 slot = commandBufferFrameIndex % (uint32)framePools.size();
        Vulkan_FramePools& fp = framePools[slot];
        vkWaitForFences(vcore.gpuDevice, 1, &fp.fence, VK_TRUE, UINT64_MAX);
        vkResetFences(vcore.gpuDevice, 1, &fp.fence);
        for (VkCommandPool pool : fp.pool)
            vkResetCommandPool(vcore.gpuDevice, pool, 0);
    }

    void EndFrame()
    {
        irisCtx.firstFrame = false;
    }
}

#endif