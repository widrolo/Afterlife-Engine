#include "Player.h"

#include "Game/Widgets/PlayerWatchWidget.h"

Player::Player()
{
    auto plWatch = std::make_shared<PlayerWatchWidget>();
    WEngine::CoreSystems::GetWidgetHandler()->AddGameWidget(plWatch);
}

void Player::Tick(float32 dt)
{
    float32 speed = m_speed * dt;
    WEngine::Vector2 move = Input::GetVector("playMove");
    WEngine::Vector2 look = Input::GetVector("playLook");

    if (Input::GetAction("sprint", PressType::Hold))
        speed *= 2.0f;

    WEngine::Vector3 moveForward = m_trans.Forward();

    float32 preY = m_trans.position.y;
    m_trans.position = m_trans.position + moveForward * move.y * speed + m_trans.Right() * move.x * speed;
    m_trans.position.y = preY;

    m_yaw += (look.x * dt * m_lookSpeed);
    m_pitch -=  (look.y * dt * m_lookSpeed) / 1.5f;

    if (m_pitch > 89.0f)
        m_pitch = 89.0f;
    if (m_pitch < -89.0f)
        m_pitch = -89.0f;

    m_trans.rotation = WEngine::Quaternion::EulerToQuaternion({glm::radians(m_pitch), glm::radians(m_yaw), 0.0f});
}

void Player::UploadCamera()
{
    WEngine::CoreSystems::GetRenderHandler()->UpdateCamera(m_trans);
}

void Player::UpdateFreecam(Freecam& freecam)
{
    freecam.m_trans = m_trans;
    freecam.m_yaw = m_yaw;
    freecam.m_pitch = m_pitch;
}
