#include "ForwardPass.h"

#include "Engine/Core/System/Iris.h"
#include "Engine/Math/Matrices/CommonMatracies.h"
#include "./Storage/ShaderStore.h"
#include "Engine/EngineDefines.h"
#include "Storage/Basics.h"
#include "Storage/Passes.h"

using namespace WEngine::Rendering;

void ForwardPass::SetupPass()
{
    Passes::forward = this;
    m_cmd = Iris::CreateCommandBuffer(Iris::QueueType::Graphics);
    auto vert = GetShader("basic", Iris::ShaderStage::Vertex);
    auto vertInst = GetShader("basicInst", Iris::ShaderStage::Vertex);
    auto frag = GetShader("basic", Iris::ShaderStage::Fragment);

    Iris::VertexLayoutDesc layout;
    AddASMFAttributes(layout);

    Iris::DepthStencilDesc depthDesc{};
    depthDesc.depthTestEnable = true;
    depthDesc.depthWriteEnable = true;
    depthDesc.depthCompareOp = Iris::CompareOp::Less;

    Iris::GraphicsPipelineDesc desc{};
    desc.debugName = "Main Pipeline";
    desc.vertexShader = vert;
    desc.fragmentShader = frag;
    desc.vertexLayout = layout;
    desc.rasterizer = Iris::RasterizerDesc{};
    desc.depthStencil = depthDesc;
    desc.blend = Iris::BlendDesc{};

    desc.tableLayouts[0] = Basics::singleTexLayout;
    desc.tableAttachmentCount = 1;

    desc.pushConstantsSize = sizeof(Mat4x4);

    m_regPipe = Iris::CreateGraphicsPipeline(desc);

    AddInstancingAttributes(layout);

    desc.vertexLayout = layout;
    desc.vertexShader = vertInst;

    m_statPipe = Iris::CreateGraphicsPipeline(desc);

    Iris::FramebufferDesc fbDesc{};
    fbDesc.hasDepth = true;
    fbDesc.width = EngineSettings::resolution.x;
    fbDesc.height = EngineSettings::resolution.y;
    fbDesc.debugName = "Primary Framebuffer";
    fbDesc.resourceTableLayout = Basics::singleTexLayout;
    fbDesc.sampler = Basics::sampler;
    m_fb = Iris::CreateFramebuffer(fbDesc);
}

void ForwardPass::Render()
{
    BeginRendering(Color(168, 233, 242), EngineSettings::resolution);
    RenderFullScene();
    EndRendering();
}
