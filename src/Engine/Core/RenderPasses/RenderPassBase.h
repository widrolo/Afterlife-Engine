#pragma once
#include "Engine/Math/Vector.h"
#include "Engine/Types/Rendering/Color.h"
#include "Engine/Types/Rendering/Iris/Descriptors.h"
#include "Engine/Types/Rendering/Iris/Handles.h"
#include "Engine/Types/Rendering/Iris/Pipeline.h"

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
        void BeginRendering(const Color& color, const Vector2& resolution);
        void EndRendering();
        void RenderFullScene();
        void AddASMFAttributes(Iris::VertexLayoutDesc& storage);
        void AddScreenAttributes(Iris::VertexLayoutDesc& storage);
        void AddInstancingAttributes(Iris::VertexLayoutDesc& storage);



    protected:
        Iris::CommandBufferHandle m_cmd = 0;
        Iris::FramebufferHandle m_fbo = 0;
        Iris::GraphicsPipelineHandle m_regPipe = 0;
        // this one includes a per object instance matrix in its vertex layout.
        Iris::GraphicsPipelineHandle m_statPipe = 0;
        Iris::FramebufferHandle m_fb;
    };
}

