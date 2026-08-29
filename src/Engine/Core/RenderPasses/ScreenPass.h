#pragma once
#include "RenderPassBase.h"

namespace WEngine::Rendering
{
    class ScreenPass : public RenderPassBase
    {
        using RenderPassBase::RenderPassBase;
    public:
        void SetupPass() override;
        void Render() override;
    };
}


