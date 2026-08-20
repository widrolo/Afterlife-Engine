#include "Viewport.h"

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

}