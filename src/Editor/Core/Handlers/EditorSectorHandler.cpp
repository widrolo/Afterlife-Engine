#include "EditorSectorHandler.h"

#include "Editor/Types/EditorSystems.h"
#include "Engine/EngineDefines.h"
#include "Engine/Core/Handlers/AssetRepo.h"
#include "Engine/Core/Handlers/RenderHandler.h"
#include "Engine/Types/CoreSystems.h"
#include "Engine/Util/Log.h"

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

void EditorSectorHandler::SaveSector(const EditorSector& sector)
{
    YAML::Node root;
    YAML::Node entries;

    for (int i = 0; i < sector.entries.size(); i++)
    {
        auto entry = SaveSingleEntry(sector.entries[i]);
        entries[std::to_string(i)] = entry;
    }

    root["sector"] = entries;
    YAML::Emitter out;
    out << root;

    std::string path = EditorSystems::GetAssetRepo()->GetDataPath();
    path += EngineSettings::sectorPath;
    path += sector.name;
    path += ".yaml";

    std::ofstream fout(path);
    fout << out.c_str();
    fout.close();

    WEngine::WLog::ConsoleLog(std::format("Saved sector \"{}\"", sector.name));
}

YAML::Node EditorSectorHandler::SaveSingleEntry(const EditorSectorEntry& entry)
{
    YAML::Node root;

    root["name"] = entry.name;
    root["asset"] = entry.asset;

    YAML::Node pos;
    pos.push_back(entry.transform.position.x);
    pos.push_back(entry.transform.position.y);
    pos.push_back(entry.transform.position.z);
    pos.SetStyle(YAML::EmitterStyle::Flow);
    root["position"] = pos;

    YAML::Node rot;
    rot.push_back(entry.transform.rotation.x);
    rot.push_back(entry.transform.rotation.y);
    rot.push_back(entry.transform.rotation.z);
    rot.push_back(entry.transform.rotation.w);
    rot.SetStyle(YAML::EmitterStyle::Flow);
    root["rotation"] = rot;

    YAML::Node size;
    size.push_back(entry.transform.size.x);
    size.push_back(entry.transform.size.y);
    size.push_back(entry.transform.size.z);
    size.SetStyle(YAML::EmitterStyle::Flow);
    root["size"] = size;

    return YAML::Clone(root);
}
