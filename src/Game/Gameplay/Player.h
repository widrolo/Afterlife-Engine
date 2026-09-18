#pragma once

#include <WidroloFramework.h>

#include "Freecam.h"

class PlayerWatchWidget;
class Player
{
    friend PlayerWatchWidget;
public:
    Player();

public:
    void Tick(float32 dt);
    void UploadCamera();
    void UpdateFreecam(Freecam& freecam);

private:
    WEngine::CharacterBodyHandle m_body;
    WEngine::Transform m_trans;
    float32 m_camHeight = 1.83f;
    float32 m_speed = 4.0f;
    float32 m_lookSpeed = 165.0f;
    float32 m_yaw = 0.0f;
    float32 m_pitch = 0.0f;
};
