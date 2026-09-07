#pragma once

#include <Engine/Types/CommonTypes.h>

namespace WEngine::Rendering
{
    /** @note Uploaded directly to the shader's std140 GTAOSettings block; keep field order in sync. */
    struct alignas(16) GTAOSettings
    {
        /** Occlusion radius in world units. Zero disables AO. */
        float32 radius = 0.5f;
        /** Fraction of radius where distance attenuation begins, clamped to [0, 0.99]. */
        float32 falloffStart = 0.5f;
        /** Occlusion multiplier. Zero disables AO; one is the unmodified result. */
        float32 strength = 1.0f;
        /** Visibility exponent. Values above one darken AO; one is neutral. */
        float32 power = 1.0f;
        /** Number of bidirectional horizon slices, clamped to [1, 16]. */
        int32 sliceCount = 4;
        /** Depth samples per side of each slice, clamped to [1, 32]. */
        int32 stepCount = 6;
        /** Screen-space search radius cap in pixels, clamped to at least one. */
        float32 maxRadiusPixels = 128.0f;
        /** Bias subtracted from horizon cosines to reduce self-occlusion, clamped to [0, 1]. */
        float32 horizonBias = 0.02f;
    };

    static_assert(sizeof(GTAOSettings) == 32);
}
