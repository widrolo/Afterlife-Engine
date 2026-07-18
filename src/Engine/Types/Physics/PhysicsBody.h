#pragma once

#include <box3d/box3d.h>

namespace WEngine
{
    struct PhysicsBody
    {
        b3BodyId bodyId;
        Entity* entity;
        PhysicsBodyType type;
    };
}