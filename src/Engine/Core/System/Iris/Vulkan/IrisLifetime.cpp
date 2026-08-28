#if GPU_BACKEND == GPU_VULKAN

#include <Engine/Core/System/Iris.h>

#include "Helpers/Helpers.h"
#include "IrisGlobals.h"
#include "Engine/Util/TimeAnalysis.h"

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
        WEngine::TimeSample sample("[Iris]BeginFrame");
        commandBufferFrameIndex++;
        const uint32 slot = commandBufferFrameIndex % (uint32)framePools.size();
        Vulkan_FramePools& fp = framePools[slot];

        for (const Vulkan_CmdBuff& cmd : loadedCommandBuffers)
            vkWaitForFences(vcore.gpuDevice, 1, &cmd.fences[slot], VK_TRUE, UINT64_MAX);
        for (const Vulkan_CmdBuff& cmd : loadedCommandBuffers)
            vkResetFences(vcore.gpuDevice, 1, &cmd.fences[slot]);

        lastSubmittedSignalSem = displayTarget.imageAvailableSems[screen.currentFrame];

        for (VkCommandPool pool : fp.pool)
            vkResetCommandPool(vcore.gpuDevice, pool, 0);
    }

    void EndFrame()
    {
        WEngine::TimeSample sample("[Iris]EndFrame");
        irisCtx.firstFrame = false;
        stats.bindStatsLastFrame = stats.bindStats;
        stats.drawStatsLastFrame = stats.drawStats;
        stats.bindStats = {};
        stats.drawStats = {};
    }
}

#endif