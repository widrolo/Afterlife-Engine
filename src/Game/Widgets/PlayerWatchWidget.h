#pragma once

#include <WidroloFramework.h>

class Player;
class PlayerWatchWidget : public WEngine::Widget
{
public:
    using Widget::Widget;
public:
    void Setup() override;
    void PassPlayer(Player* player);
protected:
    void RenderInternal() override;
private:
    Player* m_player = nullptr;
};


