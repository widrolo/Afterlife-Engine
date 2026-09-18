#include "TimeWatchWidget.h"

#include "Engine/Core/Handlers/LightTimeHandler.h"
#include "Engine/Types/CoreSystems.h"
#include "Engine/Types/Time/Date.h"

using namespace WEngine;

void TimeWatchWidget::Setup()
{
    m_widgetName = "Time Watch";
}

void TimeWatchWidget::RenderInternal()
{
    static bool timeOverride = false;
    static Time timeChanger{};

    SetSize({300, 200});

    Date date = CoreSystems::GetTimeHandler()->GetDate();
    Time time = CoreSystems::GetTimeHandler()->GetTime();

    std::string dateStr = std::format("Date: {}/{}/{}", date.GetYear(), date.GetMonth(), date.GetDay());
    std::string timeStr = std::format("Time: {}:{}:{}", time.GetHours(), time.GetMinutes(), time.GetSeconds());

    ImGui::Text("%s", dateStr.c_str());
    ImGui::Text("%s", timeStr.c_str());

    ImGui::Checkbox("Override Time", &timeOverride);

    if (timeOverride)
    {
        int32 hour = timeChanger.GetHours();
        ImGui::DragInt("Hour", &hour, 0.1, 0, 23);
        timeChanger.SetHour(hour);
        CoreSystems::GetTimeHandler()->SetTime(timeChanger);
    }
    else
    {
        timeChanger = time;
    }
}
