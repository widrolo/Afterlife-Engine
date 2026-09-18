#include "PlayerWatchWidget.h"

#include "Game/Gameplay/Player.h"

void PlayerWatchWidget::Setup()
{
    m_widgetName = "Player Watch";
}

void PlayerWatchWidget::PassPlayer(Player *player)
{
    m_player = player;
}

void PlayerWatchWidget::RenderInternal()
{
    SetSize({300, 150});
    ImGui::Text("%s", std::format("Pos: {}", m_player->m_trans.position).c_str());
}
