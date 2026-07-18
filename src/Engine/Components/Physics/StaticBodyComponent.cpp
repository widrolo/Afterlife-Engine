#include "StaticBodyComponent.h"

#include "Engine/Types/SpawnArgs.h"
#include <Engine/Core/World/Entity.h>

#include "Engine/Core/Handlers/PhysicsHandler.h"
#include "Engine/Types/CoreSystems.h"

using namespace WEngine;

REGISTER_COMPONENT(StaticBodyComponent)

StaticBodyComponent::StaticBodyComponent(Entity *e)
{
    COMP_SETUP("Static Body Component");
}

void StaticBodyComponent::Awake(ComponentArgs ca)
{
    auto* ph = CoreSystems::GetPhysicsHandler();

    m_body = ph->CreateBody(PhysicsBodyType::Static, entity);

    ph->ChangeBodyPosition(m_body, entity->transform.position);
    ph->ChangeBodyRotation(m_body, entity->transform.rotation);
}

void StaticBodyComponent::Start()
{

}

void StaticBodyComponent::PhysicsTick(float32 tr)
{

}
