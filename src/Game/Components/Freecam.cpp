#include "Freecam.h"

#include "Engine/Core/Handlers/Input.h"
#include "Engine/Core/System/Haptic.h"

const bool enabledByDefault = false;

REGISTER_COMPONENT(Freecam)

Freecam::Freecam(WEngine::Entity *e)
{
    COMP_SETUP("Freecam")
}

void Freecam::Awake(WEngine::ComponentArgs ca)
{
    m_speed = 3.0f;
    m_focused = enabledByDefault;
}

void Freecam::Start()
{
    //m_oldMousePos = input->GetMousePosition();
    //WEngine::CoreSystems::GetInputHandler()->SetMouseRelativeMode(enabledByDefault);

}

void Freecam::Tick(float32 dt)
{
    static bool firstFrame = true;
    if (firstFrame)
    {
        firstFrame = false;
        return;
    }

    //if (input->GetActionInput(WKey::DEBUG5, WEngine::Press))
    //{
    //    m_focused = !m_focused;
    //    WEngine::CoreSystems::GetInputHandler()->SetMouseRelativeMode(m_focused);
    //    firstFrame = true; // hacky solution, but it works; so meh
    //    return;
    //}

    if (dt > 10.0f)
        return;

    float32 speed = m_speed * dt;

    WEngine::Vector2 move = Input::GetVector("camMove");
    WEngine::Vector2 look = Input::GetVector("camLook");

    if (Input::GetAction("camSpeed", PressType::Hold))
        speed *= 2.0f;

    entity->transform.position = entity->transform.position + entity->transform.Forward() * move.y * speed
        + entity->transform.Right() * move.x * speed;


    if (Input::GetAction("camHigher", PressType::Hold))
        entity->transform.position.y += speed;
    if (Input::GetAction("camLower", PressType::Hold))
        entity->transform.position.y -= speed;

    //if (!m_focused)
    //    return;

    m_yaw += look.x;
    m_pitch +=  look.y / 1.5f;

    if (m_pitch > 89.0f)
        m_pitch = 89.0f;
    if (m_pitch < -89.0f)
        m_pitch = -89.0f;

    entity->transform.rotation = WEngine::Quaternion::EulerToQuaternion({glm::radians(m_pitch), glm::radians(m_yaw), 0.0f});
}
