#include "GTAOPass.h"

#include "Engine/EngineDefines.h"
#include "Engine/Core/Handlers/RenderHandler.h"
#include "Engine/Core/System/Iris.h"
#include "Engine/Math/Matrices/CommonMatracies.h"
#include "Engine/Types/CoreSystems.h"
#include "Engine/Util/TimeAnalysis.h"
#include "Storage/Basics.h"
#include "Storage/GTAOSettings.h"
#include "Storage/Passes.h"
#include "Storage/ShaderStore.h"

using namespace WEngine::Rendering;

void GTAOPass::SetupPass()
{
    Passes::gtao = this;
    m_cmd = Iris::CreateCommandBuffer(Iris::QueueType::Graphics);
    auto vert = GetShader("screen", Iris::ShaderStage::Vertex);
    auto frag = GetShader("gtao", Iris::ShaderStage::Fragment);

    Iris::ResourceTableLayoutDesc layoutDesc{};
    layoutDesc.debugName = "GTAO Settings";

    Iris::ResourceTableLayoutEntry texEntry{};
    texEntry.binding = 0;
    texEntry.stages = Iris::ShaderStage::Fragment;
    texEntry.type = Iris::ResourceTableEntryType::UniformBuffer;
    texEntry.count = 1;
    layoutDesc.entries.push_back(texEntry);

    m_settingsLayout = Iris::CreateResourceTableLayout(layoutDesc);

    Iris::VertexLayoutDesc layout;
    AddScreenAttributes(layout);

    Iris::DepthStencilDesc depthDesc;
    depthDesc.depthWriteEnable = false;

    Iris::GraphicsPipelineDesc desc{};
    desc.debugName = "GTAO Pipeline";
    desc.vertexShader = vert;
    desc.fragmentShader = frag;
    desc.vertexLayout = layout;
    desc.rasterizer = Iris::RasterizerDesc{};
    desc.topology = Iris::TopologyType::Triangle_Strip;
    desc.depthStencil = depthDesc;
    desc.blend = Iris::BlendDesc{};

    desc.tableLayouts[0] = Basics::singleTexLayout;
    desc.tableLayouts[1] = Basics::singleTexLayout;
    desc.tableLayouts[2] = m_settingsLayout;
    desc.tableAttachmentCount = 3;

    m_regPipe = Iris::CreateGraphicsPipeline(desc);

    Iris::FramebufferDesc fbDesc{};
    fbDesc.hasDepth = false;
    fbDesc.width = EngineSettings::resolution.x;
    fbDesc.height = EngineSettings::resolution.y;
    fbDesc.debugName = "GTAO Buffer";
    fbDesc.resourceTableLayout = Basics::singleTexLayout;
    fbDesc.sampler = Basics::sampler;
    m_fb = Iris::CreateFramebuffer(fbDesc);

    Iris::BufferDesc buffDesc{};
    buffDesc.debugName = "GTAO Buffer";
    buffDesc.usage = Iris::BufferUsage::Uniform;
    buffDesc.size = sizeof(GTAOSettings);
    m_settingsUniformBuffer = Iris::CreateBuffer(buffDesc, (byte*)&m_settingsData, sizeof(GTAOSettings));

    m_settings = Iris::CreateResourceTable(m_settingsLayout);

    Iris::ResourceTableUpdateDesc updateDesc{};
    Iris::ResourceTableWrite updateWrite{};
    updateWrite.buffer = m_settingsUniformBuffer;
    updateDesc.writes.push_back(updateWrite);

    Iris::UpdateResourceTable(m_settings, updateDesc);
}

GTAOSettings& GTAOPass::GetSettings()
{
    return m_settingsData;
}

void GTAOPass::UpdateSettings()
{
    auto* rh = CoreSystems::GetRenderHandler();

    m_settingsData.invProj = glm::inverse(rh->GetProjectionMatrix());
    m_settingsData.invView = glm::inverse(rh->GetViewMatrix());
    m_settingsData.viewSize = EngineSettings::resolution;
    m_settingsData.camPos = rh->GetCamera().position;
}

void GTAOPass::Render()
{
    TimeSample sample("GTAOPass::Render");

    UpdateSettings();

    Iris::UpdateBuffer(m_settingsUniformBuffer, 0, (byte*)&m_settingsData, sizeof(GTAOSettings));

    BeginRendering(Color::White, EngineSettings::resolution);

    wtl::vector<Iris::BufferHandle> vertBuffs{Basics::screenMesh};
    wtl::vector<sizeT> vertOffs{0};

    Iris::BindGraphicsPipeline(m_cmd, m_regPipe);
    Iris::BindVertexBuffers(m_cmd, 0, vertBuffs, vertOffs);

    const auto forwardFb = Passes::forward->GetFb();
    const auto normalsFb = Passes::normal->GetFb();

    Iris::BindFramebuffer(m_cmd, m_regPipe, 0, forwardFb, Iris::FramebufferBindKind::Depth);
    Iris::BindFramebuffer(m_cmd, m_regPipe, 1, normalsFb, Iris::FramebufferBindKind::Color);
    Iris::BindResourceTable(m_cmd, m_regPipe, 2, m_settings);
    Iris::Draw(m_cmd, 4, 1, 0, 0);

    EndRendering();
}
