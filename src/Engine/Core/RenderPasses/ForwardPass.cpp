#include "ForwardPass.h"

#include "Engine/Core/System/Iris.h"
#include "Engine/Math/Matrices/CommonMatracies.h"
#include "./Storage/ShaderStore.h"
#include "Engine/EngineDefines.h"
#include "Engine/Core/Handlers/RenderHandler.h"
#include "Engine/Types/CoreSystems.h"
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

    CreatePhysicsPipes();

    m_fb = CreateBasicFramebuffer("Forward", 1.0f, true);
}

void ForwardPass::Render()
{
    TimeSample sample("ForwardPass::Render");
    BeginRendering(Color(168, 233, 242), EngineSettings::resolution);

    if (!CoreSystems::GetRenderHandler()->GetPhysicsDebugSwitch())
        CoreSystems::GetRenderHandler()->RenderScene(m_cmd, m_regPipe, m_statPipe, false);
    else
        CoreSystems::GetRenderHandler()->RenderScene(m_cmd, m_phyDbgReg, m_phyDbgStat, true);

    EndRendering();
}

void ForwardPass::CreatePhysicsPipes()
{
    Iris::VertexLayoutDesc layout;
    AddAPMFAttributes(layout);

    Iris::DepthStencilDesc depthDesc{};
    depthDesc.depthTestEnable = true;
    depthDesc.depthWriteEnable = true;
    depthDesc.depthCompareOp = Iris::CompareOp::Less;

    Iris::RasterizerDesc raster{};
    raster.frontFace = Iris::FrontFace::CounterClockwise;

    Iris::GraphicsPipelineDesc desc{};
    desc.debugName = "Main Pipeline";
    desc.vertexShader = GetShader("phy", Iris::ShaderStage::Vertex);
    desc.fragmentShader = GetShader("phy", Iris::ShaderStage::Fragment);
    desc.vertexLayout = layout;
    desc.rasterizer = raster;
    desc.depthStencil = depthDesc;
    desc.blend = Iris::BlendDesc{};

    desc.pushConstantsSize = sizeof(Mat4x4) * 2;

    m_phyDbgReg = Iris::CreateGraphicsPipeline(desc);

    AddInstancingAttributes(layout);

    desc.vertexLayout = layout;
    desc.vertexShader = GetShader("phyInst", Iris::ShaderStage::Vertex);

    m_phyDbgStat = Iris::CreateGraphicsPipeline(desc);
}
