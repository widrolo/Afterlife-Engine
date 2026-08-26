#include "EditorSectorHandler.h"

#include "Engine/Core/Handlers/AssetRepo.h"
#include "Engine/Types/CoreSystems.h"

using namespace WEditor;

EditorSectorHandler::EditorSectorHandler()
{
    m_sectors = WEngine::CoreSystems::GetAssetRepo()->LoadAllEditorSectors();
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
