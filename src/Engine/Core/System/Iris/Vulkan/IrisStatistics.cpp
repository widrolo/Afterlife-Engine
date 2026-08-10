#if GPU_BACKEND == GPU_VULKAN

#include <Engine/Core/System/Iris.h>

#include "IrisGlobals.h"
#include "Helpers/Helpers.h"

namespace Iris
{
    GPUInfo GetGPUInfo()
    {
        return stats.gpuInfo;
    }
    VramStats GetVRAMStats()
    {
        return stats.vramStats;
    }
    DrawStats GetDrawCallStats()
    {
        return stats.drawStatsLastFrame;
    }
    BindStats GetBindingStats()
    {
        return stats.bindStatsLastFrame;
    }
}

#endif