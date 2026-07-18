#pragma once
#include "Engine/Components/Component.h"
#include "Engine/Types/Physics/BodyHandle.h"

namespace WEngine
{
    class BoxCollisionComponent : public Component
    {
    public:
        BoxCollisionComponent(Entity* e);

    public:
        void Awake(ComponentArgs ca) override;

        Vector3 GetSize() const { return m_size; }
        Vector3 GetOffset() const { return m_offset; }
    private:

        Vector3 m_size{};
        Vector3 m_offset{};

        COMP_HASH(0x69798e6d2538b4f9)
    };
}
