#pragma once

#include <WidroloFramework.h>

class PlayerWatchWidget : public WEngine::Widget
{
public:
    using Widget::Widget;
public:
    void Setup() override;
protected:
    void RenderInternal() override;
};


