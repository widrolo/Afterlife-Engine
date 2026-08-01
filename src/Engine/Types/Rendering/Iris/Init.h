#pragma once
#include "Engine/Math/Vector.h"
#include "Engine/Types/CommonTypes.h"

namespace Iris
{
    enum class WindowServer : uint8
    {
        SDL,
        GLFW,
        Win32,
        Apple,
        PlayStation,
        Nintendo
    };

    struct InitDesc
    {
        // This will be cast and treated to the proper window type based on the window server
        void* window = nullptr;
        WindowServer windowServer = WindowServer::SDL;

        bool enableDebug = false;
        bool enableEditorMode = false;
        uint32 framesInFlight = 2;
        bool vsync = true;
        WEngine::Vector2 preferredSwapchainSize = { 800, 600 };
    };
}
