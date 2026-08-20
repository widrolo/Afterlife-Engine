#include "EntityList.h"

#include "Editor/Types/EditorState.h"
#include "Engine/Core/World/Sector.h"
#include "Engine/Math/Transform.h"

using namespace WEditor;

void EntityList::Setup()
{
    m_widgetName = "Entity List";
    m_windowFlags = ImGuiWindowFlags_NoResize;
}

void EntityList::RenderInternal()
{
    if (EditorState::SelectedSector == nullptr)
    {
        ImGui::Text("No Sector Selected");
        return;
    }


}