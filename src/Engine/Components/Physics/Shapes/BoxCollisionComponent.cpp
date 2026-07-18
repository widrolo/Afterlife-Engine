#include "BoxCollisionComponent.h"

#include "Engine/Types/SpawnArgs.h"
#include "Engine/Core/World/Entity.h"

using namespace WEngine;

REGISTER_COMPONENT(BoxCollisionComponent)

BoxCollisionComponent::BoxCollisionComponent(Entity *e)
{
    COMP_SETUP("BoxCollisionComponent");
}

void BoxCollisionComponent::Awake(ComponentArgs ca)
{
    const auto sizeN = ca.GetVector3FromParams("size");
    const auto offsetN = ca.GetVector3FromParams("offset");

    if (sizeN.HasValue())
        m_size = sizeN.GetValue();
    if (offsetN.HasValue())
        m_offset = offsetN.GetValue();

}
