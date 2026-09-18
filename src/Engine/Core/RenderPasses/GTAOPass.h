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

        GTAOSettings& GetGTAOSettings();

    private:
        RenderPassSettings m_gtaoSettings;
        GTAOSettings m_gtaoData;
        float32 m_renderScale = 1.0f;
    };
}