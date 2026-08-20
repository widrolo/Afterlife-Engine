#pragma once
#include <Engine/WTL/vector.h>
#include <fstream>

#include <Engine/Types/CommonTypes.h>

#include "SectorEntry.h"
#include "Engine/Types/Rendering/GPU/StatBufKey.h"

namespace WEditor
{
	class Editor;
	class SectorList;
	class EntityList;
	class ComponentList;
	class ComponentSettings;
}

namespace WEngine
{
	class Sector
	{
		// i mean, at this point i might just make everything public.
		friend WEditor::Editor;
		friend WEditor::SectorList;
		friend WEditor::EntityList;
		friend WEditor::ComponentList;
		friend WEditor::ComponentSettings;
	public:
		Sector(const std::string& sectorName);

	public:
		void Show() { m_showing = true; }
		void Hide() { m_showing = false; }

		void Draw();

		[[nodiscard]] StatBufKey GetStatBufKey() const { return m_irisKey; }

	private:
		void LoadArgsFromFile(const std::string& sectorName);
	private:
		std::string m_name;
		wtl::vector<SectorEntry> m_entries;
		StatBufKey m_irisKey;

		bool m_showing = false;

		bool m_changedInEditor = false;
	};
}