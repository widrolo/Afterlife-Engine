#pragma once
#include "RenderPassBase.h"

namespace WEngine::Rendering
{
    class ForwardPass : public RenderPassBase
    {
        using RenderPassBase::RenderPassBase;
    public:
        void SetupPass() override;
        void Render() override;

    private:
        void CreatePhysicsStuff();

    private:
        Iris::GraphicsPipelineHandle m_phyDbgReg;
        Iris::GraphicsPipelineHandle m_phyDbgStat;
    };
}