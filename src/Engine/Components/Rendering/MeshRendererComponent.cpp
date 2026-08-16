#include "MeshRendererComponent.h"

#include <Engine/Types/Rendering/VertextData.h>
#include "Engine/Core/Handlers/AssetRepo.h"
#include "Engine/Core/Handlers/RenderHandler.h"
#include "Engine/Core/System/Iris.h"
#include "Engine/Types/SpawnArgs.h"
#include "Engine/Core/World/Entity.h"
#include "Engine/Core/World/Sector.h"
#include "Engine/Types/AssetMission.h"
#include "Engine/Types/CoreSystems.h"
#include "Engine/Types/Rendering/RenderMission.h"
#include "Engine/Util/Log.h"

using namespace WEngine;

REGISTER_COMPONENT(MeshRendererComponent)

MeshRendererComponent::MeshRendererComponent(Entity *e)
{
    COMP_SETUP("MeshRendererComponent")
}

void MeshRendererComponent::Awake(ComponentArgs ca)
{
    auto modelN = ca.GetStringFromParams("modelDir");

    if (modelN.HasValue())
    {
        auto meshUID = CoreSystems::GetAssetRepo()->GetFirstAssetInDirOfType(modelN.GetValue(), AssetType::StaticMesh);
        auto texUID = CoreSystems::GetAssetRepo()->GetFirstAssetInDirOfType(modelN.GetValue(), AssetType::Texture);
    }
}

void MeshRendererComponent::LateAwake()
{
    if (entity->IsStationary() && m_model != 0 && m_material != 0)
    {
        m_isStationary = true;
        CoreSystems::GetRenderHandler()->RecordStationaryAdd(entity->parentSector->GetStatBufKey(), m_model, m_material,
            entity->transform);
    }
}

void MeshRendererComponent::Draw()
{
    RenderMission mission;
    mission.transform = entity->transform;
    mission.model = m_model;
    mission.material = m_material;
    mission.isStationary = m_isStationary;
    mission.key = entity->parentSector->GetStatBufKey();

    CoreSystems::GetRenderHandler()->AddToRenderQueue(mission);

}
