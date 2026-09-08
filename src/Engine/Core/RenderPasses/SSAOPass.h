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
        Iris::ResourceTableLayoutHandle m_renSettingsLayout;
        Iris::ResourceTableHandle m_renSettings;
        Iris::ResourceTableLayoutHandle m_settingsLayout;
        Iris::ResourceTableHandle m_settings;
        Iris::BufferHandle m_renSettingsUniformBuffer;
        Iris::BufferHandle m_settingsUniformBuffer;
        RenderSettings m_settingsData;
        SSAOSettings m_ssaoData;
        float32 m_renderScale = 1.0f;
    };
}
