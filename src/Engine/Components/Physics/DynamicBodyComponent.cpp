#include "DynamicBodyComponent.h"

#include "Engine/Types/SpawnArgs.h"
#include <Engine/Core/World/Entity.h>

#include "Engine/Core/Handlers/PhysicsHandler.h"
#include "Engine/Types/CoreSystems.h"
#include "Shapes/BoxCollisionComponent.h"

using namespace WEngine;

REGISTER_COMPONENT(DynamicBodyComponent);

DynamicBodyComponent::DynamicBodyComponent(Entity *e)
{
    COMP_SETUP("Dynamic Body Component")
}

void DynamicBodyComponent::Awake(ComponentArgs ca)
{
    entity->UnlockStationary();
    auto* ph = CoreSystems::GetPhysicsHandler();

    m_body = ph->CreateBody(PhysicsBodyType::Dynamic, entity);

    ph->ChangeBodyPosition(m_body, entity->transform.position);
    ph->ChangeBodyRotation(m_body, entity->transform.rotation);
}

void DynamicBodyComponent::LateAwake()
{
    auto box = entity->GetComponent<BoxCollisionComponent>();
    if (box == nullptr)
        return;

    auto* ph = CoreSystems::GetPhysicsHandler();
    ph->AttachBox(m_body, box->GetSize(), box->GetOffset());
}

