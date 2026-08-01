#pragma once
#include <array>

#include "Descriptors.h"
#include "Handles.h"
#include "Engine/WTL/vector.h"

namespace Iris
{
    struct GraphicsPipelineDesc
    {
        wtl::vector<ShaderStageDesc> stages;
        VertexLayoutDesc vertexLayout;
        TopologyType topology = TopologyType::Triangle_List;
        bool primitiveRestartEnable = false;
        uint32 patchControlPoints = 0;
        RasterizerDesc rasterizer;
        DepthStencilDesc depthStencil;
        BlendDesc blend;

        std::array<ResourceTableLayoutHandle, 8> tableLayouts;
        uint32 colorAttachmentCount = 1;

        uint32 pushConstantsSize = 0;
        ShaderStage pushConstantsStages = ShaderStage::Vertex;

        ImgFormat colorAttachmentFormat = ImgFormat::BGRA8_UNorm;
        ImgFormat depthStencilFormat = ImgFormat::D24_UNorm_S8_UInt;
        SampleCount sampleCount = SampleCount::Samples1;

        std::string debugName;
    };

    struct ComputePipelineDesc
    {
        ShaderStageDesc stage;
        std::array<ResourceTableLayoutHandle, 8> tableLayouts;
        uint32 colorAttachmentCount = 1;
        uint32 pushConstantsSize = 0;
        ShaderStage pushConstantsStages = ShaderStage::Compute;
        std::string debugName;
    };
}
