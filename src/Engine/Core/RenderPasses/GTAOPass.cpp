#include "GTAOPass.h"

#include "Engine/EngineDefines.h"
#include "Engine/Core/Handlers/RenderHandler.h"
#include "Engine/Core/System/Iris.h"
#include "Engine/Math/Matrices/CommonMatracies.h"
#include "Engine/Stores/Steam/SteamStore.h"
#include "Engine/Types/CoreSystems.h"
#include "Engine/Util/TimeAnalysis.h"
#include "Storage/Basics.h"
#include "Storage/RenderSettings.h"
#include "Storage/Passes.h"
#include "Storage/ShaderStore.h"

using namespace WEngine::Rendering;

void GTAOPass::SetupPass()
{
    if (CoreSystems::GetSteamStore()->IsSteamDeck())
        m_renderScale = 2.0f/4.0f;

    Passes::gtao = this;
    m_cmd = Iris::CreateCommandBuffer(Iris::QueueType::Graphics);
    auto vert = GetShader("screen", Iris::ShaderStage::Vertex);
    auto frag = GetShader("gtao", Iris::ShaderStage::Fragment);

    Iris::ResourceTableLayoutDesc layoutDesc{};
    layoutDesc.debugName = "Render Settings";

    Iris::ResourceTableLayoutEntry texEntry{};
    texEntry.binding = 0;
    texEntry.stages = Iris::ShaderStage::Fragment;
    texEntry.type = Iris::ResourceTableEntryType::UniformBuffer;
    texEntry.count = 1;
    layoutDesc.entries.push_back(texEntry);

    m_renSettingsLayout = Iris::CreateResourceTableLayout(layoutDesc);

    layoutDesc.debugName = "GTAO Settings";
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
    desc.tableLayouts[2] = m_renSettingsLayout;
    desc.tableLayouts[3] = m_settingsLayout;
    desc.tableAttachmentCount = 4;

    m_regPipe = Iris::CreateGraphicsPipeline(desc);

    Iris::FramebufferDesc fbDesc{};
    fbDesc.hasDepth = false;
    fbDesc.width = EngineSettings::resolution.x * m_renderScale;
    fbDesc.height = EngineSettings::resolution.y * m_renderScale;
    fbDesc.debugName = "GTAO FB";
    fbDesc.resourceTableLayout = Basics::singleTexLayout;
    fbDesc.sampler = Basics::sampler;
    m_fb = Iris::CreateFramebuffer(fbDesc);

    Iris::BufferDesc buffDesc{};
    buffDesc.debugName = "Render Settings Buffer";
    buffDesc.usage = Iris::BufferUsage::Uniform;
    buffDesc.size = sizeof(RenderSettings);
    m_renSettingsUniformBuffer = Iris::CreateBuffer(buffDesc, (byte*)&m_settingsData, sizeof(RenderSettings));

    buffDesc.debugName = "GTAO Buffer";
    buffDesc.size = sizeof(GTAOSettings);
    m_settingsUniformBuffer = Iris::CreateBuffer(buffDesc, (byte*)&m_gtaoData, sizeof(GTAOSettings));

    m_renSettings = Iris::CreateResourceTable(m_settingsLayout);
    m_settings = Iris::CreateResourceTable(m_settingsLayout);

    Iris::ResourceTableUpdateDesc updateDesc{};
    Iris::ResourceTableWrite updateWrite{};

    updateWrite.buffer = m_renSettingsUniformBuffer;
    updateDesc.writes.push_back(updateWrite);
    Iris::UpdateResourceTable(m_renSettings, updateDesc);

    updateWrite.buffer = m_settingsUniformBuffer;
    updateDesc.writes[0] = updateWrite;
    Iris::UpdateResourceTable(m_settings, updateDesc);
}

void GTAOPass::UpdateSettings()
{
    auto* rh = CoreSystems::GetRenderHandler();

    m_settingsData.invProj = glm::inverse(rh->GetProjectionMatrix());
    m_settingsData.invView = glm::inverse(rh->GetViewMatrix());
    m_settingsData.viewSize = EngineSettings::resolution * m_renderScale;
    m_settingsData.camPos = rh->GetCamera().position;
}

void GTAOPass::Render()
{
    TimeSample sample("GTAOPass::Render");

    UpdateSettings();

    Iris::UpdateBuffer(m_renSettingsUniformBuffer, 0, (byte*)&m_settingsData, sizeof(RenderSettings));
    Iris::UpdateBuffer(m_settingsUniformBuffer, 0, (byte*)&m_gtaoData, sizeof(GTAOSettings));

    BeginRendering(Color::White, EngineSettings::resolution  * m_renderScale);

    wtl::vector<Iris::BufferHandle> vertBuffs{Basics::screenMesh};
    wtl::vector<sizeT> vertOffs{0};

    Iris::BindGraphicsPipeline(m_cmd, m_regPipe);
    Iris::BindVertexBuffers(m_cmd, 0, vertBuffs, vertOffs);

    const auto forwardFb = Passes::forward->GetFb();
    const auto normalsFb = Passes::normal->GetFb();

    Iris::BindFramebuffer(m_cmd, m_regPipe, 0, forwardFb, Iris::FramebufferBindKind::Depth);
    Iris::BindFramebuffer(m_cmd, m_regPipe, 1, normalsFb, Iris::FramebufferBindKind::Color);
    Iris::BindResourceTable(m_cmd, m_regPipe, 2, m_renSettings);
    Iris::BindResourceTable(m_cmd, m_regPipe, 3, m_settings);
    Iris::Draw(m_cmd, 4, 1, 0, 0);

    EndRendering();
}

GTAOSettings& GTAOPass::GetGTAOSettings()
{
    return m_gtaoData;
}
