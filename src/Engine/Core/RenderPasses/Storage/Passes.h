#pragma once
#include "Engine/Core/RenderPasses/ForwardPass.h"
#include "Engine/Core/RenderPasses/ScreenPass.h"
#include "Engine/Types/CommonTypes.h"

namespace WEngine::Rendering
{
    struct Passes
    {
        _GLOBAL_ ForwardPass* forward;
        _GLOBAL_ ScreenPass* screen;
    };
}
