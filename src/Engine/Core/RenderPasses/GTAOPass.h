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

        GTAOSettings& GetGTAOSettings();

    private:
        void UpdateSettings();

    private:
        RenderPassSettings m_renderSettings;
        RenderPassSettings m_gtaoSettings;
        RenderSettings m_renderData;
        GTAOSettings m_gtaoData;
        float32 m_renderScale = 1.0f;
    };
}