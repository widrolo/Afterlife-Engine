#pragma once
#include "Engine/Core/World/Sector.h"
#include "Engine/WTL/vector.h"

namespace WEngine
{
    class SectorHandler
    {
    public:
        SectorHandler();
        ~SectorHandler() = default;

    public:
        void DrawSectors();

    private:
        wtl::vector<Sector> m_sectors;
    };
}
