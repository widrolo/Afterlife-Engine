#include "Sector.h"

#include <string>

#include "Engine/Core/Handlers/RenderHandler.h"
#include "Engine/Types/CoreSystems.h"

using namespace WEngine;

Sector::Sector(const std::string& sectorName)
	: m_name(sectorName)
{

}

void Sector::Draw() const
{
	for (const auto& entry : m_entries)
	{
		RenderMission mission{};
		mission.meshUID = entry.GetMesh();
		mission.textureUID = entry.GetTexture();
		mission.transform = entry.GetTransform();
		CoreSystems::GetRenderHandler()->AddToRenderQueue(mission);
	}
}
