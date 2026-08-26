#pragma once
#include <Engine/WTL/vector.h>
#include <fstream>

#include "SectorEntry.h"
#include "Engine/Types/Rendering/RenderMission.h"
#include "Engine/Types/Rendering/Iris/Handles.h"

namespace WEditor
{
	class EntryList;
}

namespace WEngine
{
	class AssetRepo;
	class Sector
	{
		friend AssetRepo;
		friend WEditor::EntryList;
	public:
		Sector(const std::string& sectorName) : m_name(sectorName) {}

	public:
		void Show() { m_showing = true; }
		void Hide() { m_showing = false; }

		void Draw();

		[[nodiscard]] const std::string& GetName() const { return m_name; }

	private:
		std::string m_name;
		wtl::vector<SectorEntry> m_entries;
		RenderPlan m_renderPlan;

		bool m_showing = false;

		bool m_changedInEditor = false;
	};
}
