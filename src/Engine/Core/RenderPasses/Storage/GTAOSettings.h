#pragma once

#include <Engine/Types/CommonTypes.h>

namespace WEngine::Rendering
{
    /** @note Uploaded directly to the shader's std140 GTAOSettings block; keep field order in sync. */
    struct alignas(16) GTAOSettings
    {
        /** Occlusion radius in world units. Zero disables AO. */
        float32 radius = 0.14f;
        /** Fraction of radius where distance attenuation begins, clamped to [0, 0.99]. */
        float32 falloffStart = 0.16f;
        /** Occlusion multiplier. Zero disables AO; one is the unmodified result. */
        float32 strength = 1.0f;
        /** Visibility exponent. Values above one darken AO; one is neutral. */
        float32 power = 2.35f;
        /** Number of bidirectional horizon slices, clamped to [1, 16]. */
        int32 sliceCount = 6;
        /** Depth samples per side of each slice, clamped to [1, 32]. */
        int32 stepCount = 6;
        /** Screen-space search radius cap in pixels, clamped to at least one. */
        float32 maxRadiusPixels = 128.0f;
        /** Bias subtracted from horizon cosines to reduce self-occlusion, clamped to [0, 1]. */
        float32 horizonBias = 0.09f;
    };

    static_assert(sizeof(GTAOSettings) == 32);
}
