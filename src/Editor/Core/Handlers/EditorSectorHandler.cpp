#include "EditorSectorHandler.h"

#include "Engine/Core/Handlers/AssetRepo.h"
#include "Engine/Core/Handlers/RenderHandler.h"
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
    for (auto& sec : m_sectors)
    {
        for (auto& entry : sec.entries)
        {
            auto mesh = WEngine::CoreSystems::GetAssetRepo()->GetFirstAssetInDirOfType(entry.asset,
                WEngine::AssetType::StaticMesh);
            auto tex = WEngine::CoreSystems::GetAssetRepo()->GetFirstAssetInDirOfType(entry.asset,
                WEngine::AssetType::Texture);

            if (mesh == 0 || tex == 0)
            {
                mesh = WEngine::CoreSystems::GetAssetRepo()->GetFirstAssetInDirOfType("/Engine/Missing",
                    WEngine::AssetType::StaticMesh);
                tex = WEngine::CoreSystems::GetAssetRepo()->GetFirstAssetInDirOfType("/Engine/Missing",
                    WEngine::AssetType::Texture);
            }

            WEngine::RenderMission mission;
            mission.meshUID = mesh;
            mission.textureUID = tex;
            mission.transform = entry.transform;
            WEngine::CoreSystems::GetRenderHandler()->AddToRenderQueue(mission);
        }
    }
}

wtl::vector<EditorSector>& EditorSectorHandler::GetSectorList()
{
    return m_sectors;
}
