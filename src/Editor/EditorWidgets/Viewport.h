#pragma once

#include <Engine/Core/Widget.h>

#include "Engine/Core/World/Entity.h"
#include "Game/Components/Freecam.h"

namespace WEngine {
    class CameraComponent;
}

namespace WEditor
{
    class Viewport : public WEngine::Widget
    {
    public:
        using Widget::Widget;

    public:
        void Setup() override;
    protected:
        void RenderInternal() override;

    private:
        void CheckForSizeChange(ImVec2 newRes);
    private:
        WEngine::Entity* m_viewportEntity;
        WEngine::CameraComponent* viewportCam;
        Freecam* freecam;
        bool m_resDecided = false;
    };
}

