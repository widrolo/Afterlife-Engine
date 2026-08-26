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

    // everything beyond this point is pretty much filler for whenever i fix the issue of
    // sector entries not having a name

    ImGui::InputText("##", m_nameBuffer, NameBufferMaxSize);
    ImGui::SameLine();
    if (ImGui::Button("Create"))
    {
        EditorState::SelectedSector->m_entries.push_back(WEngine::SectorEntry(0, 0, 0, WEngine::Transform::Zero));
        EditorState::SelectedEntry = nullptr;
        for (sizeT i = 0; i < NameBufferMaxSize; i++)
            m_nameBuffer[i] = 0;
    }

    ImGui::SeparatorText("Entries");

    uint64 counter = 0;
    for (auto& entry : EditorState::SelectedSector->m_entries)
    {
        if (ImGui::Button(std::format("Entry {}", counter).c_str()))
        {
            EditorState::SelectedEntry = &entry;
        }
        counter++;
    }

}