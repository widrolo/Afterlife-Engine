#include "SSAOPass.h"

#include "Engine/EngineDefines.h"
#include "Engine/Core/System/Iris.h"
#include "Engine/Stores/Steam/SteamStore.h"
#include "Engine/Types/CoreSystems.h"
#include "Engine/Types/Rendering/Iris/Framebuffer.h"
#include "Engine/Util/TimeAnalysis.h"
#include "Storage/Basics.h"
#include "Storage/Passes.h"
#include "Storage/ShaderStore.h"

#include "Engine/Core/Handlers/RenderHandler.h"

using namespace WEngine::Rendering;

void SSAOPass::SetupPass()
{
    if (CoreSystems::GetSteamStore()->IsSteamDeck())
    {
        m_renderScale = 2.0f/4.0f;
        m_ssaoData.sampleCount = 16;
    }

    Passes::ssao = this;
    m_cmd = Iris::CreateCommandBuffer(Iris::QueueType::Graphics);

    m_renderSettings = CreateSettings(m_renderData, Iris::ShaderStage::Fragment, "Render");
    m_ssaoSettings = CreateSettings(m_ssaoData, Iris::ShaderStage::Fragment, "SSAO");

    m_regPipe = CreateBasicScreenPipe("ssao", {
        Basics::singleTexLayout,
        Basics::singleTexLayout,
        m_renderSettings.layout,
        m_ssaoSettings.layout
    }, "SSAO");

    m_fb = CreateBasicFramebuffer("SSAO", m_renderScale, false);
}

void SSAOPass::UpdateSettings()
{
    auto* rh = CoreSystems::GetRenderHandler();

    m_renderData.invProj = glm::inverse(rh->GetProjectionMatrix());
    m_renderData.invView = glm::inverse(rh->GetViewMatrix());
    m_renderData.viewSize = EngineSettings::resolution * m_renderScale;
    m_renderData.camPos = rh->GetCamera().position;
}

void SSAOPass::Render()
{
    TimeSample sample("SSAOPass::Render");

    UpdateSettings();

    Iris::UpdateBuffer(m_renderSettings.buffer, 0, (byte*)&m_renderData, sizeof(RenderSettings));
    Iris::UpdateBuffer(m_ssaoSettings.buffer, 0, (byte*)&m_ssaoData, sizeof(SSAOSettings));

    BeginRendering(Color::White, EngineSettings::resolution  * m_renderScale);

    wtl::vector<Iris::BufferHandle> vertBuffs{Basics::screenMesh};
    wtl::vector<sizeT> vertOffs{0};

    Iris::BindGraphicsPipeline(m_cmd, m_regPipe);
    Iris::BindVertexBuffers(m_cmd, 0, vertBuffs, vertOffs);

    const auto forwardFb = Passes::forward->GetFb();
    const auto normalsFb = Passes::normal->GetFb();

    Iris::BindFramebuffer(m_cmd, m_regPipe, 0, forwardFb, Iris::FramebufferBindKind::Depth);
    Iris::BindFramebuffer(m_cmd, m_regPipe, 1, normalsFb, Iris::FramebufferBindKind::Color);
    Iris::BindResourceTable(m_cmd, m_regPipe, 2, m_renderSettings.table);
    Iris::BindResourceTable(m_cmd, m_regPipe, 3, m_ssaoSettings.table);
    Iris::Draw(m_cmd, 4, 1, 0, 0);

    EndRendering();
}

SSAOSettings& SSAOPass::GetSSAOSettings()
{
    return m_ssaoData;
}
