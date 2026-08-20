#pragma once

#include <Engine/Core/Widget.h>


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
        bool m_resDecided = false;
    };
}

