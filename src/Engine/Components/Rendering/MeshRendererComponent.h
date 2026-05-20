#pragma once
#include "Engine/Components/Component.h"

namespace WEngine
{
    class MeshRendererComponent : public Component
    {
    public:
        MeshRendererComponent(Entity* e);

    public:
        void Awake(ComponentArgs ca) override;
        void Draw() override;

        COMP_HASH(0xeb113cecf16966)

    };
}
