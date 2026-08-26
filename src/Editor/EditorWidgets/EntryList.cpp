#include "EntryList.h"

#include "Editor/Types/EditorState.h"
#include "Engine/Core/World/Sector.h"

using namespace WEditor;

void EntryList::Setup()
{
    m_widgetName = "Entries";
    m_windowFlags = ImGuiWindowFlags_NoResize;
    m_nameBuffer = wNewArr(char, NameBufferMaxSize)
    for (sizeT i = 0; i < NameBufferMaxSize; i++)
        m_nameBuffer[i] = 0;
}

void EntryList::RenderInternal()
{
    if (EditorState::SelectedSector == nullptr)
    {
        ImGui::Text("No sector selected");
        return;
    }

    ImGui::InputText("##", m_nameBuffer, NameBufferMaxSize);
    ImGui::SameLine();
    if (ImGui::Button("Create") && m_nameBuffer[0] != 0)
    {
        EditorSectorEntry entry;
        entry.name = m_nameBuffer;
        EditorState::SelectedSector->entries.push_back(entry);
        EditorState::SelectedEntry = nullptr;
        for (sizeT i = 0; i < NameBufferMaxSize; i++)
            m_nameBuffer[i] = 0;
    }

    ImGui::SeparatorText("Entries");

    uint64 counter = 0;
    for (auto& entry : EditorState::SelectedSector->entries)
    {
        if (ImGui::Button(std::format("{}##{}", entry.name, counter).c_str()))
        {
            EditorState::SelectedEntry = &entry;
        }
        counter++;
    }

}