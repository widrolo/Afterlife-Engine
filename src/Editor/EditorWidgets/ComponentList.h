#pragma once

#include <Engine/Core/Widget.h>

#include "Engine/Core/World/Entity.h"

namespace WEditor
{
    class ComponentList : public WEngine::Widget
    {
    public:
        using Widget::Widget;

    public:
        void Setup() override;
    protected:
        void RenderInternal() override;
    private:
        const char* m_entityName;
        std::array<float32, 3> m_entityPos{};
        std::array<float32, 3> m_entityRot{};
        std::array<float32, 3> m_entitySize{};
        WEngine::Entity* m_lastEntity = nullptr;
        bool m_rotationActive = false;
    private:
        void ShowComponentsInEntity();
        void ComponentDropdown();
    };
}

