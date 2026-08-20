#pragma once

#include <WidroloFramework.h>

class Freecam
{
public:
    Freecam();

public:
    void Tick(float32 dt);
    void UploadCamera();

private:
    WEngine::Transform m_trans;
    float32 m_speed = 4.0f;
    float32 m_yaw = 0.0f;
    float32 m_pitch = 0.0f;
};

