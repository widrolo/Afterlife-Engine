#pragma once
#include "Engine/Components/Component.h"
#include "Engine/Types/Physics/BodyHandle.h"

namespace WEngine
{
    class DynamicBodyComponent : public Component
    {
    public:
        DynamicBodyComponent(Entity* e);

    public:

        void Awake(ComponentArgs ca) override;
        void Start() override;
        void PhysicsTick(float32 tr) override;

    private:
        PhysicsBodyHandle m_body;

        COMP_HASH(0x151bc1f0bf7f7e3a)
    };
}
