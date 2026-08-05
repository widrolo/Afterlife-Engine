#if GPU_BACKEND == GPU_VULKAN

#include <Engine/Core/System/Iris.h>

#include "Helpers/Helpers.h"
#include "Helpers/Init.h"
#include <SDL3/SDL_vulkan.h>

#include "IrisGlobals.h"
#include "Engine/Util/Log.h"
#include "Helpers/Allocators.h"
#include "Helpers/Swapchain.h"

namespace Iris
{
    bool Init(const InitDesc& desc)
    {
        if (!SetupInstance()) return false;
#ifndef PACKAGE
        SetupValidation();
#endif
        if (!SetupDevice()) return false;
        if (!SetupVmaAllocator()) return false;

        if (!SDL_Vulkan_CreateSurface((SDL_Window*)desc.window, vcore.instance, vcore.allocator, &screen.screen))
        {
            WEngine::WLog::SetConsoleError();
            WEngine::WLog::ConsoleLog(std::format("Failed to create a Vulkan surface for the screen, {}", SDL_GetError()));
            return false;
        }
        screen.window = desc.window;

        if (!SetupDepthImage()) return false;
        if (!SetupSwapchain()) return false;

        return true;
    }

    void Shutdown()
    {
        PrintNotImplemented("Shutdown");
    }
}



#endif