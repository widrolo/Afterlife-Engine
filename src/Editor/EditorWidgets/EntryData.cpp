#include "EntryData.h"

#include "Editor/Types/EditorState.h"

using namespace WEditor;

void EntryData::Setup()
{
    m_widgetName = "Entry Data";
	m_windowFlags = ImGuiWindowFlags_NoResize;
	m_nameBuffer = wNewArr(char, NameBufferMaxSize)
	m_assetBuffer = wNewArr(char, AssetBufferMaxSize)
	for (sizeT i = 0; i < NameBufferMaxSize; i++)
		m_nameBuffer[i] = 0;
	for (sizeT i = 0; i < AssetBufferMaxSize; i++)
		m_assetBuffer[i] = 0;
}

void EntryData::RenderInternal()
{
	if (EditorState::SelectedEntry == nullptr)
	{
		ImGui::Text("No Entry Selected");
		m_lastEntry = nullptr;
		return;
	}

	if (m_lastEntry != EditorState::SelectedEntry)
	{
		strncpy(m_nameBuffer, EditorState::SelectedEntry->name.c_str(), NameBufferMaxSize);
		strncpy(m_assetBuffer, EditorState::SelectedEntry->asset.c_str(), AssetBufferMaxSize);
		m_lastEntry = EditorState::SelectedEntry;
	}

	ImGui::InputText("##", m_nameBuffer, NameBufferMaxSize);
	ImGui::SameLine();
	if (ImGui::Button("Rename") && m_nameBuffer[0] != 0)
	{
		EditorState::SelectedEntry->name = m_nameBuffer;
	}

	ImGui::Separator();

	WEngine::Vector3& pos = EditorState::SelectedEntry->transform.position;
	WEngine::Quaternion& rot = EditorState::SelectedEntry->transform.rotation;
	WEngine::Vector3& size = EditorState::SelectedEntry->transform.size;

	WEngine::Vector3 euler = WEngine::Quaternion::QuaternionToEuler(rot);
	euler.x = glm::degrees(euler.x);
	euler.y = glm::degrees(euler.y);
	euler.z = glm::degrees(euler.z);

	// to prevent gimble lock hissy fits, we just force the editor to have full control over the rotation.
	static WEngine::Vector3 rotationGrab = WEngine::Vector3::Zero;
	static bool rotUpdate = false;

	if (rotUpdate)
		euler = rotationGrab;

	ImGui::DragFloat3("Position", (float32*)&pos, 0.1f);
	ImGui::DragFloat3("Rotation", (float32*)&euler, 0.1f);
	if (ImGui::IsItemActive())
	{
		rotUpdate = true;
		rotationGrab = euler;
	}
	if (ImGui::IsItemDeactivated())
		rotUpdate = false;

	ImGui::DragFloat3("Size", (float32*)&size, 0.1f);

	if (rotUpdate)
	{
		euler.x = glm::radians(rotationGrab.x);
		euler.y = glm::radians(rotationGrab.y);
		euler.z = glm::radians(rotationGrab.z);

		rot = WEngine::Quaternion::EulerToQuaternion(euler);
	}

	ImGui::Separator();

	ImGui::InputText("##1", m_assetBuffer, AssetBufferMaxSize);
	ImGui::SameLine();
	if (ImGui::Button("Set") && m_assetBuffer[0] != 0)
	{
		EditorState::SelectedEntry->asset = m_assetBuffer;
	}
}