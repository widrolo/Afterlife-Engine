#pragma once
#include <string>

#include "Engine/Math/Transform.h"

namespace WEditor
{
    class EditorSectorEntry
    {
    public:
        std::string name;
        std::string asset;
        WEngine::Transform transform;
    };
}
