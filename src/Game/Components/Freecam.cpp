#include "Freecam.h"

#include "Engine/Core/Handlers/Input.h"
#include "Engine/Core/System/Haptic.h"


REGISTER_COMPONENT(Freecam)

Freecam::Freecam(WEngine::Entity *e)
{
    COMP_SETUP("Freecam")
}

void Freecam::Awake(WEngine::ComponentArgs ca)
{
    m_speed = 3.0f;
}

void Freecam::Start()
{

}

void Freecam::Tick(float32 dt)
{
    static bool firstFrame = true;
    if (firstFrame)
    {
        firstFrame = false;
        return;
    }

    if (dt > 10.0f)
        return;

    float32 speed = m_speed * dt;

    WEngine::Vector2 move = Input::GetVector("camMove");
    WEngine::Vector2 look = Input::GetVector("camLook");

    if (Input::GetAction("camSpeed", PressType::Hold))
        speed *= 2.0f;

    WEngine::Vector3 moveForward = entity->transform.Forward();
    moveForward.y = -moveForward.y;

    entity->transform.position = entity->transform.position + moveForward * move.y * speed
        + entity->transform.Right() * move.x * speed;


    if (Input::GetAction("camHigher", PressType::Hold))
        entity->transform.position.y += speed;
    if (Input::GetAction("camLower", PressType::Hold))
        entity->transform.position.y -= speed;

    m_yaw += look.x;
    m_pitch +=  look.y / 1.5f;

    if (m_pitch > 89.0f)
        m_pitch = 89.0f;
    if (m_pitch < -89.0f)
        m_pitch = -89.0f;

    entity->transform.rotation = WEngine::Quaternion::EulerToQuaternion({glm::radians(m_pitch), glm::radians(m_yaw), 0.0f});
}
