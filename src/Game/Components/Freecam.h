#pragma once

#include <WidroloFramework.h>

class Freecam : public WEngine::Component
{
public:
    Freecam(WEngine::Entity* e);

public:
    void Awake(WEngine::ComponentArgs ca) override;
    void Start() override;
    void Tick(float32 dt) override;

    void EnableEditorMode() { m_editorFreecam = true; }

private:
    float32 m_speed = 3.0f;
    float32 m_yaw = 0.0f;
    float32 m_pitch = 0.0f;

    bool m_focused;
    bool m_editorFreecam = false;

    WEngine::Vector2 m_oldMousePos;

    COMP_HASH(0xe6321c612fcb459b)
};

