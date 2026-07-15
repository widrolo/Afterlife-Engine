#include <Engine/Core/Widget.h>
#include <Engine/Types/Input/InputDevice.h>
#pragma once

namespace WEngine
{

    class PeripheralWidget : public Widget
    {
    public:
        using Widget::Widget;
    private:
    public:
        void Setup() override;
    protected:
        void RenderInternal() override;
    private:
    };
} // WEngine