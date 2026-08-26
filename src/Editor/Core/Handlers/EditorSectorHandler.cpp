#include "EditorSectorHandler.h"

#include "Engine/Core/Handlers/AssetRepo.h"
#include "Engine/Types/CoreSystems.h"

using namespace WEditor;

EditorSectorHandler::EditorSectorHandler()
{
    m_sectors = WEngine::CoreSystems::GetAssetRepo()->LoadAllSectors();
}

void EditorSectorHandler::CreateSector(const std::string &secName)
{
    WEngine::Sector newSec(secName);
    newSec.Show();
    m_sectors.push_back(newSec);
}

void EditorSectorHandler::DrawSectors()
{

}

wtl::vector<WEngine::Sector>& EditorSectorHandler::GetSectorList()
{
    return m_sectors;
}
