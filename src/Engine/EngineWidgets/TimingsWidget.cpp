#include "TimingsWidget.h"
#include <Engine/Core/Engine.h>
#include <Engine/imgui/implot.h>
#include <sstream>
#include <algorithm>
#include <cmath>

#include "../Core/System/Memory.h"

using namespace WEngine;

static uint64 sampleCounter = 0;

void TimingsWidget::Setup()
{
	m_widgetName = "Engine Timings";
	m_windowFlags |= ImGuiWindowFlags_NoResize;

}

void TimingsWidget::RenderInternal()
{
	SetSize({ 400, 500 });

	sampleCounter = 0;
	PrintChildren(TimeAnalysisState::lastSample);
}

void TimingsWidget::PrintChildren(const TimeRecord &record)
{
	sampleCounter++;
	std::string nodeName = "##" + std::to_string(sampleCounter);
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_AllowOverlap;
	if (record.children.empty())
		nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	bool open = ImGui::TreeNodeEx(nodeName.c_str(), nodeFlags);

	std::string time = std::to_string((uint64)record.loggedTime) + "uS##" + std::to_string(sampleCounter);
	ImGui::SameLine();
	ImGui::Button(time.c_str());
	ImGui::SameLine();
	ImGui::Text("%s", record.sectionName.c_str());

	if (open)
	{
		for (const auto& child : record.children)
			PrintChildren(child);

		if (!record.children.empty())
			ImGui::TreePop();
	}
}
