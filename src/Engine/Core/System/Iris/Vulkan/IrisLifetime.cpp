#if GPU_BACKEND == GPU_VULKAN

#include <Engine/Core/System/Iris.h>

#include "Helpers/Helpers.h"

namespace Iris
{
    uint32 GetCurrentFrameIndex()
    {
        PrintNotImplemented("GetCurrentFrameIndex");
        return 0;
    }

    uint32 GetFramesInFlight()
    {
        PrintNotImplemented("GetFramesInFlight");
        return 0;
    }

    bool IsFirstFrame()
    {
        PrintNotImplemented("IsFirstFrame");
        return false;
    }

    void BeginFrame()
    {
        PrintNotImplemented("BeginFrame");
    }

    void EndFrame()
    {
        PrintNotImplemented("EndFrame");
    }
}

#endif