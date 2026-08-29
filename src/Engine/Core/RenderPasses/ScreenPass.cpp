#include "ScreenPass.h"

#include "Engine/EngineDefines.h"
#include "Engine/Core/System/Iris.h"
#include "Storage/Basics.h"
#include "Storage/Passes.h"
#include "Storage/ShaderStore.h"

using namespace WEngine::Rendering;

void ScreenPass::SetupPass()
{
    Passes::screen = this;
    m_cmd = Iris::CreateCommandBuffer(Iris::QueueType::Graphics);
    auto vert = GetShader("screen", Iris::ShaderStage::Vertex);
    auto frag = GetShader("screen", Iris::ShaderStage::Fragment);

    Iris::VertexLayoutDesc layout;
    AddScreenAttributes(layout);

    Iris::GraphicsPipelineDesc pipeDesc{};
    pipeDesc.debugName = "Screen Pipeline";
    pipeDesc.vertexShader = vert;
    pipeDesc.fragmentShader = frag;
    pipeDesc.vertexLayout = layout;
    pipeDesc.rasterizer = Iris::RasterizerDesc{};
    pipeDesc.topology = Iris::TopologyType::Triangle_Strip;
    pipeDesc.depthStencil = Iris::DepthStencilDesc{};
    pipeDesc.blend = Iris::BlendDesc{};

    pipeDesc.tableLayouts[0] = Basics::singleTexLayout;
    pipeDesc.tableAttachmentCount = 1;

    m_regPipe = Iris::CreateGraphicsPipeline(pipeDesc);
    m_fb = Iris::GetSwapchainFramebuffer();
}

void ScreenPass::Render()
{
    BeginRendering(Color(), EngineSettings::resolution);

    wtl::vector<Iris::BufferHandle> vertBuffs{Basics::screenMesh};
    wtl::vector<sizeT> vertOffs{0};

    Iris::BindGraphicsPipeline(m_cmd, m_regPipe);
    Iris::BindVertexBuffers(m_cmd, 0, vertBuffs, vertOffs);

    const auto forwardFb = Passes::forward->GetFb();

    Iris::BindFramebuffer(m_cmd, m_regPipe, 0, forwardFb, Iris::FramebufferBindKind::Color);
    Iris::Draw(m_cmd, 4, 1, 0, 0);

    Iris::ImGuiRenderDrawData(m_cmd);

    EndRendering();
}
