#pragma once
#include "RenderPassBase.h"
#include "Storage/GTAOSettings.h"
#include "Storage/RenderSettings.h"

namespace WEngine::Rendering
{
    class GTAOPass : public RenderPassBase
    {
        using RenderPassBase::RenderPassBase;
    public:
        void SetupPass() override;
        void Render() override;


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
        GTAOSettings m_gtaoData;
    };
}