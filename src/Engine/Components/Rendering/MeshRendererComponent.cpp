#include "MeshRendererComponent.h"

#include "Engine/Core/Handlers/RenderHandler.h"
#include "Engine/Types/SpawnArgs.h"
#include "Engine/Core/World/Entity.h"
#include "Engine/Types/CoreSystems.h"
#include "Engine/Types/Rendering/RenderMission.h"
#include "Engine/Util/Log.h"

using namespace WEngine;

REGISTER_COMPONENT(MeshRendererComponent)

MeshRendererComponent::MeshRendererComponent(Entity *e)
{
    COMP_SETUP("MeshRendererComponent")
    WLog::ConsoleLog("Cheers love");
}

void MeshRendererComponent::Awake(ComponentArgs ca)
{

}

void MeshRendererComponent::Draw()
{
    RenderMission mission;
    mission.transform = entity->transform;

    CoreSystems::GetRenderHandler()->AddToRenderQueue(mission);
}
