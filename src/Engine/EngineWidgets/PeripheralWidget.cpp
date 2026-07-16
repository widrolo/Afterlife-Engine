#include "PeripheralWidget.h"

#include <Engine/Core/Handlers/Input.h>
#include <Engine/Core/Handlers/AssetRepo.h>
#include <Engine/Types/CoreSystems.h>

#include "Engine/Core/System/Haptic.h"

using namespace WEngine;

void PeripheralWidget::Setup()
{
    m_widgetName = "Peripherals";
}

void PeripheralWidget::RenderInternal()
{
    SetSize({300, 350});

    if (ImGui::Button("Open Bindings Overlay"))
    {

    }

    auto testJPN = Haptic::GetActionJustPressed("testAction");
    auto testHeldN = Haptic::GetActionHeld("testAction");
    auto testJRN = Haptic::GetActionJustReleased("testAction");

    if (testJPN.HasValue())
    {
        if (testJPN.GetValue())
            ImGui::Text("test is just pressed");
        else
            ImGui::Text("test is NOT just pressed");
    }

    if (testHeldN.HasValue())
    {
        if (testHeldN.GetValue())
            ImGui::Text("test is held");
        else
            ImGui::Text("test is NOT held");
    }

    if (testJRN.HasValue())
    {
        if (testJRN.GetValue())
            ImGui::Text("test is just released");
        else
            ImGui::Text("test is NOT just released");
    }
}