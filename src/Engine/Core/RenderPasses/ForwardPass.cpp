#include "ForwardPass.h"

#include "Engine/Core/System/Iris.h"
#include "Engine/Math/Matrices/CommonMatracies.h"
#include "./Storage/ShaderStore.h"
#include "Engine/EngineDefines.h"
#include "Engine/Core/Handlers/AssetRepo.h"
#include "Engine/Core/Handlers/LightTimeHandler.h"
#include "Engine/Core/Handlers/RenderHandler.h"
#include "Engine/Types/CoreSystems.h"
#include "Engine/Util/TimeAnalysis.h"
#include "Storage/Basics.h"
#include "Storage/Passes.h"
#include "Storage/PhysicsDebug.h"

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
    desc.debugName = "Forward Pipeline";
    desc.vertexShader = GetShader("basic", Iris::ShaderStage::Vertex);
    desc.fragmentShader = GetShader("basic", Iris::ShaderStage::Fragment);
    desc.vertexLayout = layout;
    desc.rasterizer = Iris::RasterizerDesc{};
    desc.depthStencil = depthDesc;
    desc.blend = Iris::BlendDesc{};

    desc.tableLayouts[0] = Basics::singleTexLayout;
    desc.tableLayouts[1] = CoreSystems::GetTimeHandler()->GetLightLayoutHandle();
    desc.tableAttachmentCount = 2;

    desc.vertPushConstantsSize = sizeof(Mat4x4) * 2;

    m_regPipe = Iris::CreateGraphicsPipeline(desc);

    AddInstancingAttributes(layout);

    desc.vertexLayout = layout;
    desc.vertexShader = GetShader("basicInst", Iris::ShaderStage::Vertex);

    m_statPipe = Iris::CreateGraphicsPipeline(desc);

    CreatePhysicsStuff();
    CreateSkyPipeline();

    m_fb = CreateBasicFramebuffer("Forward", 1.0f, true);
}

void ForwardPass::Render()
{
    TimeSample sample("ForwardPass::Render");
    BeginRendering(Color(168, 233, 242), EngineSettings::resolution);

    if (!CoreSystems::GetRenderHandler()->GetPhysicsDebugSwitch())
        CoreSystems::GetRenderHandler()->RenderSkySphere(m_cmd, m_skyPipe);


    if (!CoreSystems::GetRenderHandler()->GetPhysicsDebugSwitch())
    {
        Iris::BindResourceTable(m_cmd, m_regPipe, 1, CoreSystems::GetTimeHandler()->GetLightHandle());
        CoreSystems::GetRenderHandler()->RenderScene(m_cmd, m_regPipe, m_statPipe, false);
    }
    else
    {
        Iris::BindResourceTable(m_cmd, m_phyDbgReg, 0, m_physicsColors.table);
        CoreSystems::GetRenderHandler()->RenderScene(m_cmd, m_phyDbgReg, m_phyDbgStat, true);
    }

    EndRendering();
}

void ForwardPass::CreatePhysicsStuff()
{
    m_physicsColorsData.colors[(int)PhysicsDebugColors::Static] = Color(255, 255, 255);
    m_physicsColorsData.colors[(int)PhysicsDebugColors::Dynamic] = Color(0, 255, 0);
    m_physicsColorsData.colors[(int)PhysicsDebugColors::Sleeping] = Color(252, 168, 5);
    m_physicsColors = CreateSettings(m_physicsColorsData, Iris::ShaderStage::Fragment, "Physics Debug Pass");

    Iris::VertexLayoutDesc layout;
    AddAPMFAttributes(layout);

    Iris::DepthStencilDesc depthDesc{};
    depthDesc.depthTestEnable = true;
    depthDesc.depthWriteEnable = true;
    depthDesc.depthCompareOp = Iris::CompareOp::Less;

    Iris::RasterizerDesc raster{};
    raster.frontFace = Iris::FrontFace::CounterClockwise;

    Iris::GraphicsPipelineDesc desc{};
    desc.debugName = "Physics Debug Pipeline";
    desc.vertexShader = GetShader("phy", Iris::ShaderStage::Vertex);
    desc.fragmentShader = GetShader("phy", Iris::ShaderStage::Fragment);
    desc.vertexLayout = layout;
    desc.rasterizer = raster;
    desc.depthStencil = depthDesc;
    desc.blend = Iris::BlendDesc{};

    desc.vertPushConstantsSize = sizeof(Mat4x4) * 2;

    desc.tableLayouts[0] = m_physicsColors.layout;
    desc.tableAttachmentCount = 1;

    m_phyDbgReg = Iris::CreateGraphicsPipeline(desc);

    AddInstancingAttributes(layout);

    desc.vertexLayout = layout;
    desc.vertexShader = GetShader("phyInst", Iris::ShaderStage::Vertex);

    m_phyDbgStat = Iris::CreateGraphicsPipeline(desc);
}

void ForwardPass::CreateSkyPipeline()
{
    Iris::VertexLayoutDesc layout;
    AddASMFAttributes(layout);

    Iris::RasterizerDesc rasterDesc{};
    rasterDesc.cullMode = Iris::CullMode::None;

    Iris::DepthStencilDesc depthDesc{};
    depthDesc.depthTestEnable = false;
    depthDesc.depthWriteEnable = true;
    depthDesc.depthCompareOp = Iris::CompareOp::Less;

    Iris::GraphicsPipelineDesc desc{};
    desc.debugName = "Sky Pipeline";
    desc.vertexShader = GetShader("basic", Iris::ShaderStage::Vertex);
    desc.fragmentShader = GetShader("sky", Iris::ShaderStage::Fragment);
    desc.vertexLayout = layout;
    desc.rasterizer = rasterDesc;
    desc.depthStencil = depthDesc;
    desc.blend = Iris::BlendDesc{};

    desc.tableLayouts[0] = CoreSystems::GetTimeHandler()->GetLightLayoutHandle();
    desc.tableAttachmentCount = 1;

    desc.vertPushConstantsSize = sizeof(Mat4x4) * 2;

    m_skyPipe = Iris::CreateGraphicsPipeline(desc);
}
