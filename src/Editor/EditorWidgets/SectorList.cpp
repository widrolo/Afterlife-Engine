#include "SectorList.h"

#include "Editor/Core/Handlers/EditorSectorHandler.h"
#include "Editor/Types/EditorState.h"
#include "Editor/Types/EditorSystems.h"

using namespace WEditor;

void SectorList::Setup()
{
    m_widgetName = "Sectors";
    m_windowFlags = ImGuiWindowFlags_NoResize;
    m_nameBuffer = wNewArr(char, NameBufferMaxSize)
    for (sizeT i = 0; i < NameBufferMaxSize; i++)
        m_nameBuffer[i] = 0;
}

void SectorList::RenderInternal()
{
    auto& list = EditorSystems::GetEditorSectorHandler()->GetSectorList();

    ImGui::InputText("##", m_nameBuffer, NameBufferMaxSize);
    ImGui::SameLine();
    if (ImGui::Button("Create") && m_nameBuffer[0] != 0)
    {
        EditorSystems::GetEditorSectorHandler()->CreateSector(m_nameBuffer);
        EditorState::SelectedSector = nullptr;
        EditorState::SelectedEntry = nullptr;
        for (sizeT i = 0; i < NameBufferMaxSize; i++)
            m_nameBuffer[i] = 0;
    }

    ImGui::SeparatorText("Sectors");

    uint64 counter = 0;
    for (auto& sec : list)
    {
        if (ImGui::Button(sec.name.c_str()))
        {
            if (EditorState::SelectedSector == &sec)
                continue;
            EditorState::SelectedSector = &sec;
            EditorState::SelectedEntry = nullptr;
        }
        ImGui::SameLine();
        if (ImGui::Button(std::format("Save##{}", counter).c_str()))
        {
            EditorSystems::GetEditorSectorHandler()->SaveSector(sec);
        }
        counter++;
    }

}
