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
        PrintNotImplemented("BeginFrame");
    }

    void EndFrame()
    {
        irisCtx.firstFrame = false;
        PrintNotImplemented("BeginFrame");
    }
}

#endif