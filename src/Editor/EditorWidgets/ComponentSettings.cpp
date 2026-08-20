#include "ComponentSettings.h"

#include "Editor/Core/Handlers/CompSettingsRepo.h"
#include "Editor/Types/EditorState.h"
#include "Editor/Types/EditorSystems.h"
#include "Engine/Core/World/Sector.h"
#include "Engine/Types/Rendering/Color.h"

using namespace WEditor;

void ComponentSettings::Setup()
{
    m_widgetName = "Component Settings";
	m_windowFlags = ImGuiWindowFlags_NoResize;
}

void ComponentSettings::RenderInternal()
{
    auto comp = EditorState::SelectedComponent;

    if (comp == nullptr)
    {
        ImGui::Text("No component selected");
        return;
    }
    ShowComponent();
}

void ComponentSettings::ShowComponent()
{
    //auto comp = (AnyComponent*)EditorState::SelectedComponent;
//
    //auto compDef = EditorSystems::GetCompSettingsRepo()->GetInternalOptions(comp->m_ID);
//
    //ImGui::SeparatorText(EditorSystems::GetCompSettingsRepo()->GetSettingName(comp->m_ID).c_str());
    //ImGui::Text("%s", EditorSystems::GetCompSettingsRepo()->GetSettingDesc(comp->m_ID).c_str());
//
    //int iter = 0;
	//auto& options = EditorSystems::GetCompSettingsRepo()->GetInternalOptions(comp->m_ID);
    //for (auto option : options)
    //{
    //    ShowOption(option, iter);
    //    iter++;
    //}
}

void ComponentSettings::ShowOption(const ComponentOption& option, uint8 optionNumber)
{

}
