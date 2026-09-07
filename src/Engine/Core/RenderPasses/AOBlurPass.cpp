#include "AOBlurPass.h"

#include "Engine/EngineDefines.h"
#include "Engine/Core/System/Iris.h"
#include "Engine/Util/TimeAnalysis.h"
#include "Storage/Basics.h"
#include "Storage/Passes.h"
#include "Storage/ShaderStore.h"

using namespace WEngine::Rendering;
void AOBlurPass::SetupPass()
{
    Passes::aoBlur = this;
    m_cmd = Iris::CreateCommandBuffer(Iris::QueueType::Graphics);
    auto vert = GetShader("screen", Iris::ShaderStage::Vertex);
    auto frag = GetShader("aoBlur", Iris::ShaderStage::Fragment);

    Iris::VertexLayoutDesc layout;
    AddScreenAttributes(layout);

    Iris::DepthStencilDesc depthDesc{};

    Iris::GraphicsPipelineDesc pipeDesc{};
    pipeDesc.debugName = "AO Blur Pipeline";
    pipeDesc.vertexShader = vert;
    pipeDesc.fragmentShader = frag;
    pipeDesc.vertexLayout = layout;
    pipeDesc.rasterizer = Iris::RasterizerDesc{};
    pipeDesc.topology = Iris::TopologyType::Triangle_Strip;
    pipeDesc.depthStencil = depthDesc;
    pipeDesc.blend = Iris::BlendDesc{};

    pipeDesc.tableLayouts[0] = Basics::singleTexLayout;
    pipeDesc.tableAttachmentCount = 1;

    m_regPipe = Iris::CreateGraphicsPipeline(pipeDesc);

    Iris::FramebufferDesc fbDesc{};
    fbDesc.hasDepth = false;
    fbDesc.width = EngineSettings::resolution.x;
    fbDesc.height = EngineSettings::resolution.y;
    fbDesc.debugName = "AO Blur FB";
    fbDesc.resourceTableLayout = Basics::singleTexLayout;
    fbDesc.sampler = Basics::sampler;
    m_fb = Iris::CreateFramebuffer(fbDesc);
}

void AOBlurPass::Render()
{
    TimeSample sample("AOBlurPass::Render");
    BeginRendering(Color::White, EngineSettings::resolution);

    wtl::vector<Iris::BufferHandle> vertBuffs{Basics::screenMesh};
    wtl::vector<sizeT> vertOffs{0};

    Iris::BindGraphicsPipeline(m_cmd, m_regPipe);
    Iris::BindVertexBuffers(m_cmd, 0, vertBuffs, vertOffs);

    const auto gtaoFb = Passes::gtao->GetFb();
    Iris::BindFramebuffer(m_cmd, m_regPipe, 0, gtaoFb, Iris::FramebufferBindKind::Color);

    Iris::Draw(m_cmd, 4, 1, 0, 0);

    EndRendering();
}
