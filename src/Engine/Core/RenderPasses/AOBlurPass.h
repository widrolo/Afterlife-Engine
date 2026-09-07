#pragma once
#include "RenderPassBase.h"
#include "Storage/GTAOSettings.h"
#include "Storage/RenderSettings.h"

namespace WEngine::Rendering
{
    class AOBlurPass : public RenderPassBase
    {
        using RenderPassBase::RenderPassBase;
    public:
        void SetupPass() override;
        void Render() override;

    private:
        float32 m_renderScale = 1.0f;
    };

}
