#if GPU_BACKEND == GPU_VULKAN

#include <Engine/Core/System/Iris.h>

#include "Helpers/Helpers.h"

namespace Iris
{
    sizeT GetVRAMUsage()
    {
        PrintNotImplemented("GetVRAMUsage");
        return 0;
    }

    sizeT GetDrawCallCountLastFrame()
    {
        PrintNotImplemented("GetDrawCallCountLastFrame");
        return 0;
    }
}

#endif