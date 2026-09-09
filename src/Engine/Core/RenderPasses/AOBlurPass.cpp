#include "AOBlurPass.h"

#include "Engine/EngineDefines.h"
#include "Engine/Core/System/Iris.h"
#include "Engine/Stores/Steam/SteamStore.h"
#include "Engine/Types/CoreSystems.h"
#include "Engine/Util/TimeAnalysis.h"
#include "Storage/Basics.h"
#include "Storage/Passes.h"
#include "Storage/ShaderStore.h"

using namespace WEngine::Rendering;
void AOBlurPass::SetupPass()
{
    if (CoreSystems::GetSteamStore()->IsSteamDeck())
        m_renderScale = 3.0f/4.0f;

    Passes::aoBlur = this;
    m_cmd = Iris::CreateCommandBuffer(Iris::QueueType::Graphics);

    m_regPipe = CreateBasicScreenPipe("aoBlur", {
        Basics::singleTexLayout,
    }, "AO Blur");

    m_fb = CreateBasicFramebuffer("AO Blur", m_renderScale, false);
}

void AOBlurPass::Render()
{
    TimeSample sample("AOBlurPass::Render");
    BeginRendering(Color::Black, EngineSettings::resolution * m_renderScale);

    wtl::vector<Iris::BufferHandle> vertBuffs{Basics::screenMesh};
    wtl::vector<sizeT> vertOffs{0};

    Iris::BindGraphicsPipeline(m_cmd, m_regPipe);
    Iris::BindVertexBuffers(m_cmd, 0, vertBuffs, vertOffs);

    Iris::FramebufferHandle aoFb;
    switch (GraphicsSettings::aoMethod)
    {
        case GraphicsSettings::SSAO:
            aoFb = Passes::ssao->GetFb();
            break;
        case GraphicsSettings::GTAO:
            aoFb = Passes::gtao->GetFb();
            break;
    }
    Iris::BindFramebuffer(m_cmd, m_regPipe, 0, aoFb, Iris::FramebufferBindKind::Color);

    Iris::Draw(m_cmd, 4, 1, 0, 0);

    EndRendering();
}
