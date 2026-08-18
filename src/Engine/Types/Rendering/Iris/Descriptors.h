#pragma once
#include <string>

#include "FixedFunction.h"
#include "Formats.h"
#include "Usage.h"
#include "Engine/Types/CommonTypes.h"
#include "Engine/WTL/vector.h"

namespace Iris
{
    struct BufferDesc
    {
        sizeT size;
        BufferUsage usage;
        std::string debugName;
    };

    struct TextureDesc
    {
        TextureType type = TextureType::Texture2D;
        ImgFormat format = ImgFormat::Unknown;
        uint32 width = 1;
        uint32 height = 1;
        uint32 mipLevels = 1;
        uint32 arrayLayers = 1;
        SampleCount sampleCount = SampleCount::Samples1;
        TextureUsage usage = TextureUsage::Sampled | TextureUsage::TransferDst;
        std::string debugName;
    };

    struct SamplerDesc
    {
        FilterMode magFilter = FilterMode::Linear;
        FilterMode minFilter = FilterMode::Linear;
        MipFilterMode mipFilter = MipFilterMode::Linear;
        SamplerAddressMode addressU = SamplerAddressMode::Repeat;
        SamplerAddressMode addressV = SamplerAddressMode::Repeat;
        float32 mipLodBias = 0.0f;
        // Not relevant, would cause more issues than it fixed otherwise I only speak
        // for Vulkan here, maybe DX12, Metal or AGC are starving for this info?
        //float32 minLod = 0.0f;
        //float32 maxLod = 0.0f;
        float32 maxAnisotropy = 1.0f;
        bool anisotropyEnable = false;
        CompareOp compareOp = CompareOp::Never;
        bool compareEnable = false;
        BorderColor borderColor = BorderColor::TransparentBlack;
        std::string debugName;
    };

    struct ShaderStageDesc
    {
        ShaderStage stage;
        const dword* bytecode;
        sizeT bytecodeSize;
        std::string entryPoint = "main";
        std::string debugName;
    };


    struct VertexAttributeDesc
    {
        uint32 location;
        uint32 binding;
        VertFormat format;
    };

    struct VertexBindingDesc
    {
        uint32 binding;
        uint32 stride;
        bool perInstance;
    };

    struct VertexLayoutDesc
    {
        wtl::vector<VertexAttributeDesc> attributes;
        wtl::vector<VertexBindingDesc> bindings;
    };

    struct RasterizerDesc
    {
        CullMode cullMode = CullMode::Front;
        FrontFace frontFace = FrontFace::CounterClockwise;
        FillMode fillMode = FillMode::Solid;
        float32 lineWidth = 1.0f;
        bool depthClampEnable = false;
        bool depthBiasEnable = false;
        float32 depthBiasConstant = 0.0f;
        float32 depthBiasClamp = 0.0f;
        float32 depthBiasSlope = 0.0f;
    };

    struct StencilFaceDesc
    {
        StencilOp failOp = StencilOp::Keep;
        StencilOp passOp = StencilOp::Keep;
        StencilOp depthFailOp = StencilOp::Keep;
        CompareOp compareOp = CompareOp::Always;
        byte writeMask = max_byte;
        byte compareMask = max_byte;
    };

    struct DepthStencilDesc
    {
        bool depthTestEnable = false;
        bool depthWriteEnable = true;
        CompareOp depthCompareOp = CompareOp::Less;
        bool stencilTestEnable = false;
        StencilFaceDesc front;
        StencilFaceDesc back;
    };

    struct BlendAttachmentDesc
    {
        bool blendEnable = false;
        BlendFactor srcColorFactor = BlendFactor::One;
        BlendFactor dstColorFactor = BlendFactor::Zero;
        BlendOp colorOp = BlendOp::Add;
        BlendFactor srcAlphaFactor = BlendFactor::One;
        BlendFactor dstAlphaFactor = BlendFactor::Zero;
        BlendOp alphaOp = BlendOp::Add;
        byte colorWriteMask  = 0xF;  // 0 | 0 | 0 | 0 | R | G | B | A
    };

    struct BlendDesc
    {
        // "An idiot admires complexity, a genius admires simplicity" - Terry Davis
        bool enableBlending = false;


        //bool logicOpEnable = false;
        //LogicOp logicOp = LogicOp::Noop;
        //wtl::vector<BlendAttachmentDesc> attachments;
        //float32 blendConstants[4] = {0,0,0,0};
    };
}
