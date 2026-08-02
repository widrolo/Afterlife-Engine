#if GPU_BACKEND == GPU_VULKAN

#include <Engine/Core/System/Iris.h>

#include "Helpers/Helpers.h"

namespace Iris
{
    TextureHandle AcquireSwapchainTexture()
    {
        PrintNotImplemented("AcquireSwapchainTexture");
        return 0;
    }

    FramebufferHandle GetSwapchainFramebuffer()
    {
        PrintNotImplemented("GetSwapchainFramebuffer");
        return 0;
    }

    void Present()
    {
        PrintNotImplemented("Present");
    }
}
#endif