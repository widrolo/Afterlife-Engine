#pragma once

#include <WidroloFramework.h>

#include "Freecam.h"

class Player
{
public:
    Player();

public:
    void Tick(float32 dt);
    void UploadCamera();
    void UpdateFreecam(Freecam& freecam);

private:
    WEngine::Transform m_trans;
    float32 m_speed = 4.0f;
    float32 m_lookSpeed = 165.0f;
    float32 m_yaw = 0.0f;
    float32 m_pitch = 0.0f;
};
