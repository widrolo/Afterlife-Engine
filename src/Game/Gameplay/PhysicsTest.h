#pragma once

#include <WidroloFramework.h>

struct PhysicsTestBody
{
    WEngine::Transform transform;
    WEngine::PhysicsBodyHandle body;
};

class PhysicsTest
{
public:
    PhysicsTest();

public:
    void Tick(float32 dt);
    void Draw();

private:
    uint32 m_mesh;
    uint32 m_texture;
    uint32 m_collider;

    wtl::vector<PhysicsTestBody> m_testBodies;
};
