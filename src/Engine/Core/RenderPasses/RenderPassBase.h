#pragma once
#include "Engine/Core/System/Iris.h"
#include "Engine/Math/Vector.h"
#include "Engine/Types/Rendering/Color.h"
#include "Engine/Types/Rendering/Iris/Descriptors.h"
#include "Engine/Types/Rendering/Iris/Handles.h"
#include "Engine/Types/Rendering/Iris/Pipeline.h"
#include "Engine/Types/Rendering/Iris/Resource.h"

namespace WEngine::Rendering
{
    class RenderPassBase
    {
    public:
        RenderPassBase();
        virtual ~RenderPassBase() = default;

    public:
        virtual void SetupPass() = 0;
        virtual void Render() = 0;

        [[nodiscard]] Iris::FramebufferHandle GetFb() const { return m_fb; }

    protected:
        struct RenderPassSettings
        {
            Iris::ResourceTableLayoutHandle layout;
            Iris::ResourceTableHandle table;
            Iris::BufferHandle buffer;
        };

    protected:
        void BeginRendering(const Color& color, const Vector2& resolution);
        void EndRendering();
        void RenderFullScene(bool noTex = false);
        void AddASMFAttributes(Iris::VertexLayoutDesc& storage);
        void AddScreenAttributes(Iris::VertexLayoutDesc& storage);
        void AddInstancingAttributes(Iris::VertexLayoutDesc& storage);
        Iris::FramebufferHandle CreateBasicFramebuffer(const std::string& passName = "Pass", float32 renderScale = 1.0f,
            bool hasDepth = false);
        Iris::GraphicsPipelineHandle CreateBasicScreenPipe(const std::string& fragShader,
            const wtl::vector<Iris::ResourceTableLayoutHandle>& layouts, const std::string& passName = "Pass");


        template<typename T>
        RenderPassSettings CreateSettings(const T& initData, Iris::ShaderStage stage, const std::string& passName = "Pass")
        {
            Iris::ResourceTableLayoutDesc layoutDesc{};
            layoutDesc.debugName = std::format("{} Settings", passName);

            Iris::ResourceTableLayoutEntry texEntry{};
            texEntry.binding = 0;
            texEntry.stages = stage;
            texEntry.type = Iris::ResourceTableEntryType::UniformBuffer;
            texEntry.count = 1;
            layoutDesc.entries.push_back(texEntry);

            auto layout = Iris::CreateResourceTableLayout(layoutDesc);
            auto table = Iris::CreateResourceTable(layout);

            Iris::BufferDesc buffDesc{};
            buffDesc.debugName = std::format("{} Settings Buffer", passName);
            buffDesc.usage = Iris::BufferUsage::Uniform;
            buffDesc.size = sizeof(T);
            auto buf = Iris::CreateBuffer(buffDesc, (byte*)&initData, sizeof(T));

            Iris::ResourceTableUpdateDesc updateDesc{};
            Iris::ResourceTableWrite updateWrite{};

            updateWrite.buffer = buf;
            updateDesc.writes.push_back(updateWrite);
            Iris::UpdateResourceTable(table, updateDesc);
            return {layout, table, buf};
        }


    protected:
        Iris::CommandBufferHandle m_cmd = 0;
        Iris::FramebufferHandle m_fbo = 0;
        Iris::GraphicsPipelineHandle m_regPipe = 0;
        // this one includes a per object instance matrix in its vertex layout.
        Iris::GraphicsPipelineHandle m_statPipe = 0;
        Iris::FramebufferHandle m_fb;
    };
}

