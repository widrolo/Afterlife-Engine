#include "PeripheralWidget.h"

#include <Engine/Core/Handlers/InputHandler.h>
#include <Engine/Core/Handlers/AssetRepo.h>
#include <Engine/Types/CoreSystems.h>

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
        CoreSystems::GetInputHandler()->OpenConfigPanel();
    }
}