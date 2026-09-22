#pragma once

#include <Engine/Core/RenderPasses/RenderPassBase.h>

namespace WEditor::Rendering
{
    class ObjIDPass : public WEngine::Rendering::RenderPassBase
    {
        using RenderPassBase::RenderPassBase;
    public:
        void SetupPass() override;
        void Render() override;
    };

}
