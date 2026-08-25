#pragma once

#include <Engine/Core/Widget.h>
#include <Engine/WTL/deque.h>
#include <Engine/Types/CommonTypes.h>

namespace WEngine
{
	class TimingsWidget : public Widget
	{
	public:
		using Widget::Widget;
	public:
		void Setup() override;
	protected:
		void RenderInternal() override;
	};
}

