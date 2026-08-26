#include "EditorSectorHandler.h"

#include "Engine/Core/Handlers/AssetRepo.h"
#include "Engine/Types/CoreSystems.h"

using namespace WEditor;

EditorSectorHandler::EditorSectorHandler()
{
    // TODO: write a editor specific loader for editor sectors in asset repo.
    //m_sectors = WEngine::CoreSystems::GetAssetRepo()->LoadAllSectors();
}

void EditorSectorHandler::CreateSector(const std::string &secName)
{
    EditorSector newSec;
    newSec.name = secName;
    m_sectors.push_back(newSec);
}

void EditorSectorHandler::DrawSectors()
{

}

wtl::vector<EditorSector>& EditorSectorHandler::GetSectorList()
{
    return m_sectors;
}
