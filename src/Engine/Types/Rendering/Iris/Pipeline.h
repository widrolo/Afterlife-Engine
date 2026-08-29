#pragma once
#include <array>

#include "Descriptors.h"
#include "Handles.h"
#include "Engine/WTL/vector.h"

namespace Iris
{
    struct GraphicsPipelineDesc
    {
        ShaderHandle vertexShader;
        ShaderHandle fragmentShader;

        VertexLayoutDesc vertexLayout;
        TopologyType topology = TopologyType::Triangle_List;
        RasterizerDesc rasterizer;
        DepthStencilDesc depthStencil;
        BlendDesc blend;

        std::array<ResourceTableLayoutHandle, 8> tableLayouts;
        uint32 tableAttachmentCount = 0;

        uint32 pushConstantsSize = 0;
        ShaderStage pushConstantsStage = ShaderStage::Vertex;

        ImgFormat colorAttachmentFormat = ImgFormat::BGRA8_UNorm;
        ImgFormat depthStencilFormat = ImgFormat::D32_SFloat_S8_UInt;
        SampleCount sampleCount = SampleCount::Samples1;

        std::string debugName;
    };

    struct ComputePipelineDesc
    {
        ShaderHandle computeShader;
        std::array<ResourceTableLayoutHandle, 8> tableLayouts;
        uint32 tableAttachmentCount = 1;
        uint32 pushConstantsSize = 0;
        std::string debugName;
    };
}
