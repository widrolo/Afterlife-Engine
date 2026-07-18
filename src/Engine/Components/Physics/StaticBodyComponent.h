#pragma once
#include "Engine/Components/Component.h"
#include "Engine/Types/Physics/BodyHandle.h"

namespace WEngine
{
    class StaticBodyComponent : public Component
    {
    public:
        StaticBodyComponent(Entity* e);

    public:

        void Awake(ComponentArgs ca) override;
        void Start() override;
        void PhysicsTick(float32 tr) override;

    private:
        PhysicsBodyHandle m_body;

        COMP_HASH(0x8a48613b13971d89)
    };
}

