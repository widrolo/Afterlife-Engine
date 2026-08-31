#pragma once

#include <glm/glm.hpp>

namespace WEngine::Rendering
{
    struct GTAOSettings
    {
        alignas(16) Vector3 camPos;
        alignas(16) glm::mat4 invProj;
        alignas(16) glm::mat4 invView;
        alignas(16) Vector2 viewSize;
    };
}
