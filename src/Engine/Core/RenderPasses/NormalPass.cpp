#include "NormalPass.h"

#include "Engine/EngineDefines.h"
#include "Engine/Core/System/Iris.h"
#include "Engine/Math/Matrices/CommonMatracies.h"
#include "Engine/Util/TimeAnalysis.h"
#include "Storage/Basics.h"
#include "Storage/Passes.h"
#include "Storage/ShaderStore.h"

using namespace WEngine::Rendering;

void NormalPass::SetupPass()
{
    Passes::normal = this;
    m_cmd = Iris::CreateCommandBuffer(Iris::QueueType::Graphics);
    auto vert = GetShader("basic", Iris::ShaderStage::Vertex);
    auto vertInst = GetShader("basicInst", Iris::ShaderStage::Vertex);
    auto frag = GetShader("normals", Iris::ShaderStage::Fragment);

    Iris::VertexLayoutDesc layout;
    AddASMFAttributes(layout);

    Iris::DepthStencilDesc depthDesc{};
    depthDesc.depthTestEnable = true;
    depthDesc.depthWriteEnable = true;
    depthDesc.depthCompareOp = Iris::CompareOp::Less;

    Iris::GraphicsPipelineDesc desc{};
    desc.debugName = "Normals Pipeline";
    desc.vertexShader = vert;
    desc.fragmentShader = frag;
    desc.vertexLayout = layout;
    desc.rasterizer = Iris::RasterizerDesc{};
    desc.depthStencil = depthDesc;
    desc.blend = Iris::BlendDesc{};

    desc.pushConstantsSize = sizeof(Mat4x4) * 2;

    m_regPipe = Iris::CreateGraphicsPipeline(desc);

    AddInstancingAttributes(layout);

    desc.vertexLayout = layout;
    desc.vertexShader = vertInst;

    m_statPipe = Iris::CreateGraphicsPipeline(desc);

    Iris::FramebufferDesc fbDesc{};
    fbDesc.hasDepth = true;
    fbDesc.width = EngineSettings::resolution.x;
    fbDesc.height = EngineSettings::resolution.y;
    fbDesc.debugName = "Normal G-Buffer";
    fbDesc.resourceTableLayout = Basics::singleTexLayout;
    fbDesc.sampler = Basics::sampler;
    m_fb = Iris::CreateFramebuffer(fbDesc);
}

void NormalPass::Render()
{
    TimeSample sample("NormalPass::Render");
    BeginRendering(Color::Black, EngineSettings::resolution);
    RenderFullScene(true);
    EndRendering();
}
