#include "TimingsWidget.h"
#include <Engine/Core/Engine.h>
#include <Engine/imgui/implot.h>
#include <sstream>
#include <algorithm>
#include <cmath>

#include "../Core/System/Memory.h"

using namespace WEngine;

void TimingsWidget::Setup()
{
	m_widgetName = "Engine Timings";
	m_windowFlags |= ImGuiWindowFlags_NoResize;

}

void TimingsWidget::RenderInternal()
{
	SetSize({ 400, 500 });

}
