#pragma once
#include "Engine/Types/Rendering/Color.h"

namespace WEngine::Rendering
{
    enum class PhysicsDebugColors
    {
        Static,
        Dynamic,
        Sleeping,

        PhysicsDebugColors_Count
    };
    struct alignas(16) PhysicsDebugColorsData
    {
        Colorf colors[(sizeT)PhysicsDebugColors::PhysicsDebugColors_Count];
    };
}
