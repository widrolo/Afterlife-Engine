#include "SectorHandler.h"

#include "AssetRepo.h"
#include "Engine/Types/CoreSystems.h"

using namespace WEngine;

SectorHandler::SectorHandler()
{
    m_sectors = CoreSystems::GetAssetRepo()->LoadAllSectors();
}

void SectorHandler::DrawSectors()
{
    for (const auto& sec : m_sectors)
        sec.Draw();
}
