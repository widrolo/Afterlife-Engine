#pragma once
#include "Editor/Core/World/EditorSector.h"
#include "Engine/Core/World/Sector.h"
#include "Engine/WTL/vector.h"

#include <yaml-cpp/yaml.h>

namespace WEditor
{
    class EditorSectorHandler
    {
    public:
        EditorSectorHandler();
        ~EditorSectorHandler() = default;

    public:
        void CreateSector(const std::string& secName);
        void DrawSectors();
        wtl::vector<EditorSector>& GetSectorList();

        void SaveSector(const EditorSector& sector);

    private:
        YAML::Node SaveSingleEntry(const EditorSectorEntry& entry);

    private:
        wtl::vector<EditorSector> m_sectors;
    };
}

