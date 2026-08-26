#pragma once
#include "Editor/Core/World/EditorSector.h"
#include "Engine/Core/World/Sector.h"
#include "Engine/WTL/vector.h"


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

    private:
        wtl::vector<EditorSector> m_sectors;
    };
}

