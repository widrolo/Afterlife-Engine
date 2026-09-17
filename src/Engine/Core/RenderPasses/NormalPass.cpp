#include "NormalPass.h"

#include "Engine/EngineDefines.h"
#include "Engine/Core/System/Iris.h"
#include "Engine/Math/Matrices/CommonMatracies.h"
#include "Engine/Core/Handlers/RenderHandler.h"
#include "Engine/Types/CoreSystems.h"
#include "Engine/Util/TimeAnalysis.h"
#include "Storage/Basics.h"
#include "Storage/Passes.h"
#include "Storage/ShaderStore.h"

using namespace WEngine::Rendering;

void NormalPass::SetupPass()
{
    Passes::normal = this;
    m_cmd = Iris::CreateCommandBuffer(Iris::QueueType::Graphics);

    Iris::VertexLayoutDesc layout;
    AddASMFAttributes(layout);

    Iris::DepthStencilDesc depthDesc{};
    depthDesc.depthTestEnable = true;
    depthDesc.depthWriteEnable = true;
    depthDesc.depthCompareOp = Iris::CompareOp::Less;

    Iris::GraphicsPipelineDesc desc{};
    desc.debugName = "Normals Pipeline";
    desc.vertexShader = GetShader("basic", Iris::ShaderStage::Vertex);
    desc.fragmentShader = GetShader("normals", Iris::ShaderStage::Fragment);
    desc.vertexLayout = layout;
    desc.rasterizer = Iris::RasterizerDesc{};
    desc.depthStencil = depthDesc;
    desc.blend = Iris::BlendDesc{};

    desc.pushConstantsSize = sizeof(Mat4x4) * 2;

    m_regPipe = Iris::CreateGraphicsPipeline(desc);

    AddInstancingAttributes(layout);

    desc.vertexLayout = layout;
    desc.vertexShader = GetShader("basicInst", Iris::ShaderStage::Vertex);

    m_statPipe = Iris::CreateGraphicsPipeline(desc);

    CreatePhysicsPipes();

    m_fb = CreateBasicFramebuffer("Normals", 1.0f, true);
}

void NormalPass::Render()
{
    TimeSample sample("NormalPass::Render");
    BeginRendering(Color::Black, EngineSettings::resolution);

    if (!CoreSystems::GetRenderHandler()->GetPhysicsDebugSwitch())
        CoreSystems::GetRenderHandler()->RenderScene(m_cmd, m_regPipe, m_statPipe, true);
    else
        CoreSystems::GetRenderHandler()->RenderScene(m_cmd, m_phyDbgReg, m_phyDbgStat, true);

    EndRendering();
}

void NormalPass::CreatePhysicsPipes()
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
    desc.debugName = "Physics Debug Normals Pipeline";
    desc.vertexShader = GetShader("phy", Iris::ShaderStage::Vertex);
    desc.fragmentShader = GetShader("phyNormals", Iris::ShaderStage::Fragment);
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
