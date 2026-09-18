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

        void SetDate(const Date& date);
        void SetTime(const Time& time);

        [[nodiscard]] Date GetDate() const;
        [[nodiscard]] Time GetTime() const;

        [[nodiscard]] Iris::ResourceTableLayoutHandle GetLightLayoutHandle() const { return m_layoutHandle; }
        [[nodiscard]] Iris::ResourceTableHandle GetLightHandle() const { return m_lightHandle; }

        SunLightInfo& GetSunLightInfo() { return m_worldLighting->sun; }
        Colorf& GetAmbientColor() { return m_worldLighting->ambient; }
        float32& GetAmbientIntensity() { return m_worldLighting->ambientIntensity; }

    private:
        void UpdateRenderTime();
        Vector3 CalcSunDir(float32 timeFactor);
        void UploadLighting();

        void SetLightDefaults();

    private:
        Date m_date;
        Time m_time;
        float32 m_accumulator;
        WorldLighting* m_worldLighting;
        Iris::ResourceTableLayoutHandle m_layoutHandle;
        Iris::ResourceTableHandle m_lightHandle;
        Iris::BufferHandle m_lightBuffer;
    };
}
