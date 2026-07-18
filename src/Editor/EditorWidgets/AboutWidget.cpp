#include "AboutWidget.h"

#include <Engine/Util/Log.h>
using namespace WEditor;

void AboutWidget::Setup()
{
    m_widgetName = "About";
    m_windowFlags |= ImGuiWindowFlags_NoResize;
}

void AboutWidget::RenderInternal()
{
    SetSize({400, 350});

    ImGui::Text("[A]fterlife [T]ool[K]it");
}
