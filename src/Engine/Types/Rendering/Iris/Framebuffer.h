#pragma once
#include <string>

#include "Handles.h"
#include "Engine/WTL/vector.h"

namespace Iris
{
    struct FramebufferDesc
    {
        uint32 width;
        uint32 height;
        uint32 layers = 1;
        bool hasDepth;
        std::string debugName;
    };
}
