#include "SectorEntry.h"

#include "Engine/Core/Handlers/AssetRepo.h"
#include "Engine/Types/CoreSystems.h"

using namespace WEngine;

SectorEntry::SectorEntry(const std::string& assetName)
{
    m_mesh = CoreSystems::GetAssetRepo()->GetFirstAssetInDirOfType(assetName, AssetType::StaticMesh);
    m_texture = CoreSystems::GetAssetRepo()->GetFirstAssetInDirOfType(assetName, AssetType::Texture);
    // Physics meshes are not yet supported.
}
