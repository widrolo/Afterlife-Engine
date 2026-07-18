#pragma once

#include <Engine/Types/CommonTypes.h>

namespace WEngine
{
    /**
     * Handle for a physics body.
     */
    struct PhysicsBodyHandle
    {
        PhysicsBodyHandle() : handle(0) {};
        PhysicsBodyHandle(uint64 handle) : handle(handle) {}
        operator uint64() const { return handle; }
        uint64 handle;
    };
}