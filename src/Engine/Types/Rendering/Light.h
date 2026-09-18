#pragma once
#include <Engine/Types/CommonTypes.h>
#include <Engine/Math/Vector.h>

#include <cstddef>

#include "Color.h"

namespace WEngine
{
    // this is the hard cap, the engine wont crash past this, but lights will be cut off.
    // we shouldnt toy around with this since we use forward non plus rendering.
    constexpr sizeT MaxLights = 15; // actually 16 because of the sun light.

    enum class LightType
    {
        Omni,
        Spot,
    };

    struct alignas(16) LightData
    {
        alignas(16) Vector3 position;
        alignas(16) Vector3 direction;
        alignas(16) Colorf color;
        float32 intensity;
        float32 range;
        float32 innerCone;
        float32 outerCone;
    };

    struct alignas(16) SunLightInfo
    {
        alignas(16) Vector3 direction;
        alignas(16) Colorf color;
        float32 intensity;
    };

    struct LightSource
    {
        LightType type;
        LightData data;
    };

    // this is the info we send to the GPU each frame.
    struct alignas(16) WorldLighting
    {
        alignas(16) SunLightInfo sun;
        alignas(16) Colorf ambient;
        float32 ambientIntensity;
        uint32 numLights;
        LightSource sources[MaxLights];
    };
}
