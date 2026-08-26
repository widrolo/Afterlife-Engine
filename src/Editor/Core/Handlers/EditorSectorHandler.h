#pragma once
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
        wtl::vector<WEngine::Sector>& GetSectorList();

    private:
        wtl::vector<WEngine::Sector> m_sectors;
    };
}

