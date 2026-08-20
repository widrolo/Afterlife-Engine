#pragma once
#include <Engine/WTL/vector.h>
#include <fstream>

#include <Engine/Types/CommonTypes.h>

#include "SectorEntry.h"
#include "Engine/Types/Rendering/Iris/Handles.h"


namespace WEngine
{
	class AssetRepo;
	class Sector
	{
		friend AssetRepo;
	public:
		Sector(const std::string& sectorName);

	public:
		void Show() { m_showing = true; }
		void Hide() { m_showing = false; }

		void Draw() const;

		[[nodiscard]] Iris::BufferHandle GetStatBufKey() const { return m_statBuffer; }
		[[nodiscard]] const std::string& GetName() const { return m_name; }

	private:
		std::string m_name;
		wtl::vector<SectorEntry> m_entries;
		Iris::BufferHandle m_statBuffer;

		bool m_showing = false;

		bool m_changedInEditor = false;
	};
}
