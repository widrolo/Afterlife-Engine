#pragma once

#include <glm/glm.hpp>

namespace WEngine::Rendering
{
    struct GTAOSettings
    {
        Vector3 camPos;
        glm::mat4 invProj;
        glm::mat4 invView;
        Vector2 viewSize;
    };
}
