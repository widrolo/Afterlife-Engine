#pragma once
#include "Engine/Math/Vector.h"
#include "Engine/Types/CommonTypes.h"
#include "Engine/Types/Rendering/Light.h"
#include "Engine/Types/Rendering/Iris/Handles.h"
#include "Engine/Types/Time/Date.h"
#include "Engine/Types/Time/Time.h"

namespace WEngine
{
    class LightTimeHandler
    {
    public:
        LightTimeHandler();

    public:
        void SetupLighting();
        void Update(float32 dt);
        void UploadLighting();

        void SetDate(const Date& date);
        void SetTime(const Time& time);

        [[nodiscard]] Date GetDate() const;
        [[nodiscard]] Time GetTime() const;

        [[nodiscard]] Iris::ResourceTableLayoutHandle GetLightLayoutHandle() const { return m_layoutHandle; }
        [[nodiscard]] Iris::ResourceTableHandle GetLightHandle() const { return m_lightHandle; }

        SunLightInfo& GetSunLightInfo() { return m_worldLighting->sun; }
        Colorf& GetAmbientColor() { return m_worldLighting->ambient; }
        float32& GetAmbientIntensity() { return m_worldLighting->ambientIntensity; }

        RenderSettings& GetRenderSettings() { return m_renderSettings; }
        void SetLightDefaultsEditor();

    private:
        void UpdateRenderTime();
        Vector3 CalcSunDir(float32 timeFactor);

        void SetLightDefaults();

    private:
        Date m_date;
        Time m_time;
        float32 m_accumulator;
        WorldLighting* m_worldLighting;
        RenderSettings m_renderSettings;
        Iris::ResourceTableLayoutHandle m_layoutHandle;
        Iris::ResourceTableHandle m_lightHandle;
        Iris::BufferHandle m_lightBuffer;
        Iris::BufferHandle m_renderSettBuffer;
    };
}
