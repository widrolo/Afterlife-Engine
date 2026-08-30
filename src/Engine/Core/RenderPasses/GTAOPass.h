#pragma once
#include "RenderPassBase.h"
#include "Storage/GTAOSettings.h"

namespace WEngine::Rendering
{
    class GTAOPass : public RenderPassBase
    {
        using RenderPassBase::RenderPassBase;
    public:
        void SetupPass() override;
        void Render() override;

        GTAOSettings& GetSettings();

    private:
        void UpdateSettings();

    private:
        Iris::ResourceTableLayoutHandle m_settingsLayout;
        Iris::ResourceTableHandle m_settings;
        Iris::BufferHandle m_settingsUniformBuffer;
        GTAOSettings m_settingsData;
    };
}