#include "ForwardPass.h"

#include "Engine/Core/System/Iris.h"
#include "Engine/Math/Matrices/CommonMatracies.h"
#include "./Storage/ShaderStore.h"
#include "Engine/EngineDefines.h"
#include "Engine/Util/TimeAnalysis.h"
#include "Storage/Basics.h"
#include "Storage/Passes.h"

using namespace WEngine::Rendering;

void ForwardPass::SetupPass()
{
    Passes::forward = this;
    m_cmd = Iris::CreateCommandBuffer(Iris::QueueType::Graphics);

    Iris::VertexLayoutDesc layout;
    AddASMFAttributes(layout);

    Iris::DepthStencilDesc depthDesc{};
    depthDesc.depthTestEnable = true;
    depthDesc.depthWriteEnable = true;
    depthDesc.depthCompareOp = Iris::CompareOp::Less;

    Iris::GraphicsPipelineDesc desc{};
    desc.debugName = "Main Pipeline";
    desc.vertexShader = GetShader("basic", Iris::ShaderStage::Vertex);
    desc.fragmentShader = GetShader("basic", Iris::ShaderStage::Fragment);
    desc.vertexLayout = layout;
    desc.rasterizer = Iris::RasterizerDesc{};
    desc.depthStencil = depthDesc;
    desc.blend = Iris::BlendDesc{};

    desc.tableLayouts[0] = Basics::singleTexLayout;
    desc.tableAttachmentCount = 1;

    desc.pushConstantsSize = sizeof(Mat4x4) * 2;

    m_regPipe = Iris::CreateGraphicsPipeline(desc);

    AddInstancingAttributes(layout);

    desc.vertexLayout = layout;
    desc.vertexShader = GetShader("basicInst", Iris::ShaderStage::Vertex);

    m_statPipe = Iris::CreateGraphicsPipeline(desc);

    m_fb = CreateBasicFramebuffer("Forward", 1.0f, true);
}

void ForwardPass::Render()
{
    TimeSample sample("ForwardPass::Render");
    BeginRendering(Color(168, 233, 242), EngineSettings::resolution);
    RenderFullScene();
    EndRendering();
}
