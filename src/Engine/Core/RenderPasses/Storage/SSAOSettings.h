#pragma once

#include <Engine/Types/CommonTypes.h>

namespace WEngine::Rendering
{
    /** @note Uploaded directly to the shader's std140 SSAOSettings block; keep field order in sync. */
    struct alignas(16) SSAOSettings
    {
        /** Hemisphere radius in world units. Zero disables AO. */
        float32 radius = 0.58f;
        /** Depth and surface-normal bias in world units, clamped to at least zero. */
        float32 bias = 0.002f;
        /** Occlusion multiplier. Zero disables AO; one is the unmodified result. */
        float32 strength = 1.0f;
        /** Visibility exponent. Values above one darken AO; zero disables AO. */
        float32 power = 2.7f;
        /** Number of hemisphere depth samples, clamped to [1, 64]. */
        int32 sampleCount = 32;
        /** Fraction of radius where distance attenuation begins, clamped to [0, 0.99]. */
        float32 falloffStart = 0.0f;
        /** Matches the shader's trailing vec2; not a setting. */
        float32 padding[2] = {};
    };

    static_assert(sizeof(SSAOSettings) == 32);
}
