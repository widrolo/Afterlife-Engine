#include "WidgetHandler.h"

#include <algorithm>
#include <Engine/EngineWidgets/SystemWidget.h>
#include <Engine/EngineWidgets/EngineControlWidget.h>
#include <Engine/EngineWidgets/StatisticsWidgets.h>
#include <Engine/EngineWidgets/TimingsWidget.h>
#include <Engine/EngineWidgets/PeripheralWidget.h>
#include <Engine/EngineWidgets/GameSystemWidget.h>
#include <Engine/EngineWidgets/SectorWatchWidget.h>
#include <Engine/EngineWidgets/PhysicsWatchWidget.h>
#include <Engine/EngineWidgets/DebugFlagsWidget.h>

#include <Engine/Types/CoreSystems.h>

#include <Engine/Core/System/Memory.h>

#include <Engine/Core/System/Haptic.h>
#include <Engine/EngineWidgets/RenderWatchWidget.h>
#include <Engine/EngineWidgets/TimeWatchWidget.h>
#include <Engine/Util/Log.h>
using namespace WEngine;

WidgetHandler::WidgetHandler()
{
    InitSystemWidgets();
}

WidgetHandler::~WidgetHandler()
{
    for (const auto& widget : m_systemWidgets)
    {
        if (widget != nullptr)
            WAllocator::Destruct(widget);
    }
}

void WidgetHandler::InitSystemWidgets()
{
    m_systemWidgets[(sizeT)SysWidgetTypes::System]         = WAllocator::Construct<SystemWidget>();
    m_systemWidgets[(sizeT)SysWidgetTypes::EngineControl]  = WAllocator::Construct<EngineControlWidget>();
    m_systemWidgets[(sizeT)SysWidgetTypes::GameSystem]     = WAllocator::Construct<GameSystemWidget>();
    m_systemWidgets[(sizeT)SysWidgetTypes::Statistics]     = WAllocator::Construct<StatisticsWidgets>();
    m_systemWidgets[(sizeT)SysWidgetTypes::Timings]        = WAllocator::Construct<TimingsWidget>();
    m_systemWidgets[(sizeT)SysWidgetTypes::Peripherals]    = WAllocator::Construct<PeripheralWidget>();
    m_systemWidgets[(sizeT)SysWidgetTypes::SectorWatch]    = WAllocator::Construct<SectorWatchWidget>();
    m_systemWidgets[(sizeT)SysWidgetTypes::PhysicsWatch]   = WAllocator::Construct<PhysicsWatchWidget>();
    m_systemWidgets[(sizeT)SysWidgetTypes::DebugFlags]     = WAllocator::Construct<DebugFlagsWidget>();
    m_systemWidgets[(sizeT)SysWidgetTypes::RenderWatch]    = WAllocator::Construct<RenderWatchWidget>();
    m_systemWidgets[(sizeT)SysWidgetTypes::TimeWatch]      = WAllocator::Construct<TimeWatchWidget>();


    for (sizeT i = 0; i < (sizeT)SysWidgetTypes::SysWidget_Count; i++)
    {
        if (m_systemWidgets[i] == nullptr)
            continue;
        m_systemWidgets[i]->Setup();
    }

    auto* stat =    m_systemWidgets[(sizeT)SysWidgetTypes::Statistics];
    auto* control = m_systemWidgets[(sizeT)SysWidgetTypes::EngineControl];
    auto* sys =     m_systemWidgets[(sizeT)SysWidgetTypes::System];
    auto* gameSys = m_systemWidgets[(sizeT)SysWidgetTypes::GameSystem];

    if (stat != nullptr)    stat->m_open = true;
    if (control != nullptr) control->m_open = true;
    if (sys != nullptr)     sys->m_open = true;
    if (gameSys != nullptr) gameSys->m_open = true;
}

void WidgetHandler::DrawWidgets()
{
    auto* stat =    m_systemWidgets[(sizeT)SysWidgetTypes::Statistics];
    auto* control = m_systemWidgets[(sizeT)SysWidgetTypes::EngineControl];
    auto* sys =     m_systemWidgets[(sizeT)SysWidgetTypes::System];
    auto* gameSys = m_systemWidgets[(sizeT)SysWidgetTypes::GameSystem];

    if (Haptic::GetDebugKeyJustPressed(1) && sys != nullptr)
        sys->m_open = !sys->m_open;

    if (Haptic::GetDebugKeyJustPressed(2) && gameSys != nullptr)
        gameSys->m_open = !gameSys->m_open;

    if (Haptic::GetDebugKeyJustPressed(12))
    {
        m_widgetsEnabled = !m_widgetsEnabled;
        if (m_widgetsEnabled)
        {
            if (stat != nullptr)    stat->m_open = true;
            if (control != nullptr) control->m_open = true;
            if (sys != nullptr)     sys->m_open = true;
            if (gameSys != nullptr) gameSys->m_open = true;
        }
        else
        {
            for (sizeT i = 0; i < (sizeT)SysWidgetTypes::SysWidget_Count; i++)
            {
                if (m_systemWidgets[i] == nullptr)
                    continue;
                m_systemWidgets[i]->m_open = false;
            }
            for (const auto& widget : m_gameWidgets)
            {
                if (const auto& lw = widget.lock())
                    lw->m_open = false;
            }
        }
    }

    for (sizeT i = 0; i < (sizeT)SysWidgetTypes::SysWidget_Count; i++)
    {
        if (m_systemWidgets[i] == nullptr)
            continue;
        if (m_systemWidgets[i]->m_open)
            m_systemWidgets[i]->RenderWidget();
    }

    auto* time = static_cast<TimingsWidget*>(m_systemWidgets[(sizeT)SysWidgetTypes::Timings]);

    if (time != nullptr)
        time->CountTime();

    // im trying everything vro, this shit will soon be nasa grade code.
    //auto widgetSnapshot = m_gameWidgets; // no longer crashes, but im gonna keep this commented for now.
    for (const auto& weak : m_gameWidgets)
    {
        if (const auto& widget = weak.lock())
        {
            widget->RenderWidget();
        }
    }
}

void WidgetHandler::AddGameWidget(const std::shared_ptr<Widget>& widget)
{
    widget->Setup();
    m_gameWidgets.push_back(widget);
}

void WidgetHandler::RemoveGameWidget(const std::shared_ptr<Widget>& widget)
{
    const auto it = std::ranges::remove_if(m_gameWidgets,
        [&widget](const std::weak_ptr<Widget>& weak)
        {
            auto locked = weak.lock();
            return !locked || locked == widget;
        }).begin();


    m_gameWidgets.erase(it, m_gameWidgets.end());
}
