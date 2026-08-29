#include "RenderPassBase.h"

#include "Engine/Core/Handlers/RenderHandler.h"
#include "Engine/Core/System/Iris.h"
#include "Engine/Math/Matrices/CommonMatracies.h"
#include "Engine/Types/CoreSystems.h"

using namespace WEngine::Rendering;

RenderPassBase::RenderPassBase()
{

}

void RenderPassBase::BeginRendering(const Color &color, const Vector2 &resolution)
{
    Iris::BeginCommandBuffer(m_cmd);

    Iris::RenderPassBeginDesc desc{};
    desc.colorAttachment.clearColor = color;
    desc.framebuffer = m_fb;
    Iris::BeginRenderPass(m_cmd, desc);

    Iris::Viewport vp{};
    vp.extent = resolution;
    vp.maxDepth = 1.0f;
    Iris::SetViewport(m_cmd, vp);
}

void RenderPassBase::EndRendering()
{
    Iris::EndRenderPass(m_cmd);
    Iris::EndCommandBuffer(m_cmd);
    Iris::SubmitCommandBuffer(m_cmd);
}

void RenderPassBase::RenderFullScene(bool noTex)
{
    CoreSystems::GetRenderHandler()->RenderScene(m_cmd, m_regPipe, m_statPipe, noTex);
}

void RenderPassBase::AddASMFAttributes(Iris::VertexLayoutDesc &storage)
{
    uint32 bindStart = storage.bindings.size();
    uint32 attrStart = storage.attributes.size();

    // the asmf file thats loaded in is laid out like this:
    // |     Name     |  Size  |
    // |--------------|--------|
    // |Position      |12 bytes|
    // |Normal        |12 bytes|
    // |UV            | 8 bytes|

    Iris::VertexBindingDesc binding{};
    binding.binding = bindStart;
    binding.perInstance = false;
    binding.stride = 32;
    storage.bindings.push_back(binding);

    Iris::VertexAttributeDesc attribute{};
    attribute.binding = bindStart;
    attribute.location = attrStart;
    attribute.format = Iris::VertFormat::Float3;

    storage.attributes.push_back(attribute);

    attribute.location = attrStart + 1;
    storage.attributes.push_back(attribute);

    attribute.location = attrStart + 2;
    attribute.format = Iris::VertFormat::Float2;
    storage.attributes.push_back(attribute);
}

void RenderPassBase::AddScreenAttributes(Iris::VertexLayoutDesc &storage)
{
    uint32 bindStart = storage.bindings.size();
    uint32 attrStart = storage.attributes.size();

    Iris::VertexBindingDesc binding{};
    binding.binding = bindStart;
    binding.perInstance = false;
    binding.stride = sizeof(float32) * 4;
    storage.bindings.push_back(binding);

    Iris::VertexAttributeDesc attribute{};
    attribute.binding = bindStart;
    attribute.location = attrStart;
    attribute.format = Iris::VertFormat::Float2;
    storage.attributes.push_back(attribute);

    attribute.location = attrStart + 1;
    storage.attributes.push_back(attribute);
}

void RenderPassBase::AddInstancingAttributes(Iris::VertexLayoutDesc &storage)
{
    uint32 bindStart = storage.bindings.size();
    uint32 attrStart = storage.attributes.size();

    Iris::VertexBindingDesc binding{};
    binding.binding = bindStart;
    binding.perInstance = true;
    binding.stride = sizeof(Mat4x4);

    Iris::VertexAttributeDesc attribute{};
    storage.bindings.push_back(binding);

    attribute.binding = bindStart;
    attribute.format = Iris::VertFormat::Float4;
    // this is still the stupidest way to send a matrix to the gpu.
    attribute.location = attrStart;
    storage.attributes.push_back(attribute);
    attribute.location = attrStart + 1;
    storage.attributes.push_back(attribute);
    attribute.location = attrStart + 2;
    storage.attributes.push_back(attribute);
    attribute.location = attrStart + 3;
    storage.attributes.push_back(attribute);
}

