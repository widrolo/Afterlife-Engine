#if GPU_BACKEND == GPU_VULKAN

#include <Engine/Core/System/Iris.h>

#include "Helpers/Helpers.h"

namespace Iris
{
    void ConfigureImGui()
    {
        PrintNotImplemented("ConfigureImGui");
    }

    void ImGuiNewFrame()
    {
        PrintNotImplemented("ImGuiNewFrame");
    }

    void ImGuiEndFrame()
    {
        PrintNotImplemented("ImGuiEndFrame");
    }

    void ImGuiRenderDrawData(CommandBufferHandle cmd)
    {
        PrintNotImplemented("ImGuiRenderDrawData");
    }

    WEngine::Nullable<ImTextureID> TextureToImGui(TextureHandle texture)
    {
        PrintNotImplemented("TextureToImGui");
        return {};
    }
}


#endif