#pragma once

#include <box3d/box3d.h>

namespace WEngine
{
    struct PhysicsBody
    {
        b3BodyId bodyId;
        Transform* transform;
        PhysicsBodyType type;
    };
    struct CharacterBody
    {
        b3BodyId bodyId;
        b3Capsule mover;
        
    };
}