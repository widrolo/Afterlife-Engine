#include "LightTimeHandler.h"

#include <cmath>
#include <numbers>

#include "RenderHandler.h"
#include "Engine/EngineDefines.h"
#include "Engine/Core/System/Iris.h"
#include "Engine/Types/CoreSystems.h"
#include "Engine/Types/Rendering/Iris/Resource.h"
#include "Engine/Util/Log.h"
#include "Engine/Util/TimeAnalysis.h"

using namespace WEngine;

LightTimeHandler::LightTimeHandler()
{
    m_date = Date(TimeSettings::startYear, TimeSettings::startMonth, TimeSettings::startDay);
    m_time = Time(TimeSettings::startHour, TimeSettings::startMinute, TimeSettings::startSecond);
    m_accumulator = 0.0f;

    m_worldLighting = wNewArr(WorldLighting, 1)
    memset(m_worldLighting, 0, sizeof(WorldLighting));
    SetLightDefaults();

}

void LightTimeHandler::SetupLighting()
{
    static bool everRan = false;
    if (everRan)
        return;
    everRan = true;

    Iris::ResourceTableLayoutDesc layoutDesc{};
    layoutDesc.debugName = "World Lighting Layout";

    Iris::ResourceTableLayoutEntry lightEntry{};
    lightEntry.binding = 0;
    lightEntry.stages = Iris::ShaderStage::Fragment;
    lightEntry.type = Iris::ResourceTableEntryType::UniformBuffer;
    lightEntry.count = 1;

    layoutDesc.entries.push_back(lightEntry);

    lightEntry.binding = 1;
    lightEntry.stages = Iris::ShaderStage::Fragment;
    lightEntry.type = Iris::ResourceTableEntryType::UniformBuffer;
    lightEntry.count = 1;

    layoutDesc.entries.push_back(lightEntry);

    m_layoutHandle = Iris::CreateResourceTableLayout(layoutDesc);
    m_lightHandle = Iris::CreateResourceTable(m_layoutHandle);

    Iris::BufferDesc buffDesc{};
    buffDesc.debugName = "World Lighting Buffer";
    buffDesc.usage = Iris::BufferUsage::Uniform;
    buffDesc.size = sizeof(WorldLighting);
    m_lightBuffer = Iris::CreateBuffer(buffDesc, (byte*)m_worldLighting, sizeof(WorldLighting));
    buffDesc.debugName = "Render Settings Buffer";
    buffDesc.usage = Iris::BufferUsage::Uniform;
    buffDesc.size = sizeof(RenderSettings);
    m_renderSettBuffer = Iris::CreateBuffer(buffDesc, (byte*)&m_renderSettings, sizeof(RenderSettings));

    Iris::ResourceTableUpdateDesc updateDesc{};

    Iris::ResourceTableWrite updateWrite{};
    updateWrite.buffer = m_lightBuffer;
    updateDesc.writes.push_back(updateWrite);

    updateWrite.binding = 1;
    updateWrite.buffer = m_renderSettBuffer;
    updateDesc.writes.push_back(updateWrite);

    Iris::UpdateResourceTable(m_lightHandle, updateDesc);
}

void LightTimeHandler::Update(float32 dt)
{
    TimeSample sample("TimeHandler::Update");
    m_accumulator += dt * TimeSettings::gameSecondPerRealSecond;
    uint32 secs = std::floor(m_accumulator);
    m_accumulator -= (float32)secs;
    uint32 days = m_time.AddSeconds(secs);
    m_date.AddDays(days);

    UpdateRenderTime();
}

void LightTimeHandler::SetDate(const Date &date)
{
    m_date = date;
}

void LightTimeHandler::SetTime(const Time &time)
{
    m_time = time;
}

Date LightTimeHandler::GetDate() const
{
    return m_date;
}

Time LightTimeHandler::GetTime() const
{
    return m_time;
}

void LightTimeHandler::UpdateRenderTime()
{
    TimeSample sample("TimeHandler::UpdateRenderTime");
    constexpr uint32 secondsInDay = 60 * 60 * 24;

    uint32 secs = m_time.GetSeconds();
    secs += m_time.GetMinutes() * 60;
    secs += m_time.GetHours() * 60 * 60;

    float32 timeFactor = (float32)secs / (float32)secondsInDay;

    m_worldLighting->timeFactor = timeFactor;
    m_worldLighting->sun.direction = CalcSunDir(timeFactor);
    m_worldLighting->sun.intensity = std::fmaxf(-std::pow(timeFactor - 0.533, 4) * 110 + 1, 0.0);

    // pls keep this identical to the shader
    float32 timeFacAmb = tanh(2*sin((timeFactor - 2*std::numbers::pi) * 2*std::numbers::pi)) / 2.1 + (0.524);
    m_worldLighting->ambientIntensity = -timeFacAmb / 10.0f + 0.2f;
}

Vector3 LightTimeHandler::CalcSunDir(float32 timeFactor)
{
    constexpr float32 axialTilt = 23.5f * (std::numbers::pi / 180.0f);
    const Vector3 axis = Vector3(cosf(axialTilt), sinf(axialTilt), 0.0f);

    const Vector3 start = Vector3(0.0f, 0.0f, 1.0f);

    const float32 phaseOffset = std::numbers::pi * 0.5f;
    const float32 theta = timeFactor * 2.0f * std::numbers::pi + phaseOffset;
    const float32 cosT  = cosf(theta);
    const float32 sinT  = sinf(theta);

    const float32 dot = VecMath::Dot(axis, start);
    const Vector3 cross = VecMath::Cross(axis, start);

    return start * cosT + cross * sinT + axis * dot * (1.0f - cosT);
}

void LightTimeHandler::UploadLighting()
{
    Iris::UpdateBuffer(m_lightBuffer, 0, (byte*)m_worldLighting, sizeof(WorldLighting));
    Iris::UpdateBuffer(m_renderSettBuffer, 0, (byte*)&m_renderSettings, sizeof(RenderSettings));
}

void LightTimeHandler::SetLightDefaults()
{
    m_worldLighting->sun.color = Color(255, 243, 195);
    m_worldLighting->sun.direction = Vector3(0.0f, 1.0f, 0.0f);
    m_worldLighting->sun.intensity = 100.0f;
    m_worldLighting->ambient = Color(150, 175, 255);
}
