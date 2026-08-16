#pragma once
#include "Engine/WTL/vector.h"
#include <Engine/Types/Rendering/VertextData.h>
#include <string>

namespace WEngine
{
    struct VertexData;

    struct MeshInfo
    {
        sizeT vertexOffset;
        sizeT vertexSize;
        sizeT indexOffset;
        sizeT indexSize;
    };
}
