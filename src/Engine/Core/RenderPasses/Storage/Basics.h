#pragma once
#include "Engine/Types/CommonTypes.h"
#include "Engine/Types/Rendering/Iris/Handles.h"

namespace WEngine::Rendering
{
    struct Basics
    {
        _GLOBAL_ Iris::SamplerHandle sampler;
        _GLOBAL_ Iris::ResourceTableLayoutHandle singleTexLayout;
        _GLOBAL_ Iris::BufferHandle screenMesh;
    };
}
