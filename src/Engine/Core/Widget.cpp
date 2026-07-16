#include "Widget.h"

#include <cmath>
#include <iostream>

#include "Engine/Types/CoreSystems.h"
#include "Engine/Util/Log.h"
#include "Handlers/RenderHandler.h"

using namespace WEngine;

Widget::Widget()
{
	m_widgetName = "No Name Widget";
	m_windowFlags = 0;
	m_open = false;

	Setup();
}

void Widget::RenderWidget()
{
	if (!m_open)
		return;
	ImGui::Begin(m_widgetName.c_str(), &m_open, m_windowFlags);

	RenderInternal();

	ImGui::End();
}

void Widget::Setup()
{


}

void Widget::RenderInternal()
{
}

void Widget::SetPosition(const Vector2 &position)
{
	ImGui::SetWindowPos({position.x, position.y});
}
void Widget::SetSize(const Vector2 &size)
{
	ImGui::SetWindowSize({size.x, size.y}, 0);
}

void Widget::DequeToCArray(wtl::deque<float32> &buf, uint16 maxSize, float32 *outData)
{
	sizeT count = (sizeT)std::min(buf.size(), static_cast<size_t>(maxSize));
	for (sizeT i = 0; i < count; ++i)
		outData[i] = buf[i];

	// NaN the rest if deque isnt full
	for (sizeT i = count; i < maxSize; ++i)
		outData[i] = NAN;
}
