#include "Viewport.h"

#include "Engine/Core/Handlers/RenderHandler.h"
#include "Engine/Core/System/Iris.h"

#include "Game/Gameplay/Freecam.h"

using namespace WEditor;

void Viewport::Setup()
{
    m_widgetName = "Viewport";
    m_windowFlags = ImGuiWindowFlags_NoResize;
}

void Viewport::RenderInternal()
{
    Iris::FramebufferHandle fb = WEngine::CoreSystems::GetRenderHandler()->EditorGetViewportFramebuffer();
    auto image = Iris::GetFramebufferImGuiImage(fb);
    ImGui::Image(image.GetValue(), ImGui::GetContentRegionAvail());
}