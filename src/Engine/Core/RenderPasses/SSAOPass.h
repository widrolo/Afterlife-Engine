#pragma once
#include "RenderPassBase.h"
#include "Storage/RenderSettings.h"
#include "Storage/SSAOSettings.h"

namespace WEngine::Rendering
{
    class SSAOPass : public RenderPassBase
    {
        using RenderPassBase::RenderPassBase;
    public:
        void SetupPass() override;
        void Render() override;

        SSAOSettings& GetSSAOSettings();

    private:
        void UpdateSettings();

    private:
        RenderPassSettings m_renderSettings;
        RenderPassSettings m_ssaoSettings;

        RenderSettings m_renderData;
        SSAOSettings m_ssaoData;
        float32 m_renderScale = 1.0f;
    };
}
