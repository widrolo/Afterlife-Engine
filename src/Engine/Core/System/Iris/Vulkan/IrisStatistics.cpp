#if GPU_BACKEND == GPU_VULKAN

#include <Engine/Core/System/Iris.h>

#include "IrisGlobals.h"
#include "Helpers/Helpers.h"

namespace Iris
{
    sizeT GetVRAMUsage()
    {
        return stats.vramUsage;
    }

    sizeT GetDrawCallCountLastFrame()
    {
        return stats.drawCallsLastFrame;
    }
}

#endif