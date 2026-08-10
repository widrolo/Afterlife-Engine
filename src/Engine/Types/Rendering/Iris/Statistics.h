#pragma once
#include "Engine/Types/CommonTypes.h"

namespace Iris
{
    struct GPUInfo
    {
        std::string gpuName;
        sizeT totalVram;
    };

    struct VramStats
    {
        sizeT total;

        sizeT vertexBuffers;
        sizeT indexBuffers;
        sizeT uniformBuffer;
        sizeT storageBuffers;
        sizeT transferBuffers;

        sizeT colorTextures;
        sizeT depthTextures;
        sizeT framebuffers;

        sizeT vertexShader;
        sizeT geometryShader;
        sizeT tesselationShader;
        sizeT fragmentShaders;
        sizeT computeShaders;
    };

    struct BindStats
    {
        sizeT total;

        sizeT graphicsPipelineBinds;
        sizeT computePipelineBinds;
        sizeT vertexBinds;
        sizeT indexBinds;
        sizeT tableBinds;
    };

    struct DrawStats
    {
        sizeT total;

        sizeT draw;
        sizeT drawIndexed;
        sizeT drawIndirect;
        sizeT drawIndirectIndexed;

        sizeT dispatch;
        sizeT dispatchIndirect;

        sizeT copyBuffToBuff;
        sizeT copyBuffToTex;
    };
}
