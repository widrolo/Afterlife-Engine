#include "Player.h"

#include "Game/Widgets/PlayerWatchWidget.h"


Player::Player()
{
    auto plWatch = std::make_shared<PlayerWatchWidget>();
    plWatch->PassPlayer(this);
    WEngine::CoreSystems::GetWidgetHandler()->AddGameWidget(plWatch);

    b3Capsule mover;
    mover.center1 = (b3Vec3){ 0.0f, 0.0f, 6.0f };  // bottom sphere center
    mover.center2 = (b3Vec3){ 0.0f, 1.85f, 6.0f };  // top sphere center
    mover.radius  = 0.35f;

    m_body = WEngine::CoreSystems::GetPhysicsHandler()->CreateCharacter(mover);
}

void Player::Tick(float32 dt)
{
    float32 speed = m_speed * dt;
    WEngine::Vector2 move = Input::GetVector("playMove");
    WEngine::Vector2 look = Input::GetVector("playLook");

    if (Input::GetAction("sprint", PressType::Hold))
        speed *= 2.0f;

    WEngine::Quaternion yawRotation = WEngine::Quaternion::EulerToQuaternion({0.0f, glm::radians(m_yaw), 0.0f});
    WEngine::Vector3 moveForward = WEngine::Quaternion::Rotate(yawRotation, {0.0f, 0.0f, -1.0f});
    WEngine::Vector3 moveRight = WEngine::Quaternion::Rotate(yawRotation, {1.0f, 0.0f, 0.0f});

    WEngine::Vector3 position = m_trans.position;
    float32 preY = m_trans.position.y;
    position = m_trans.position + moveForward * move.y * speed + moveRight * move.x * speed;
    position.y = preY;

    WEngine::CoreSystems::GetPhysicsHandler()->MoveCharacter(m_body, position - m_trans.position);
    m_trans.position = WEngine::CoreSystems::GetPhysicsHandler()->GetCharacterPosition(m_body);

    position = m_trans.position;
    position.y -= 5.0f * dt; // fools gravity
    WEngine::CoreSystems::GetPhysicsHandler()->MoveCharacter(m_body, position - m_trans.position);
    m_trans.position = WEngine::CoreSystems::GetPhysicsHandler()->GetCharacterPosition(m_body);

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
    WEngine::Transform camTrans = m_trans;
    camTrans.position.y += m_camHeight;
    WEngine::CoreSystems::GetRenderHandler()->UpdateCamera(camTrans);
}

void Player::UpdateFreecam(Freecam& freecam)
{
    freecam.m_trans = m_trans;
    freecam.m_yaw = m_yaw;
    freecam.m_pitch = m_pitch;
}
