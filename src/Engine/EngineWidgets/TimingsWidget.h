#pragma once

#include <Engine/Core/Widget.h>
#include <Engine/WTL/deque.h>
#include <Engine/Types/CommonTypes.h>

#include "Engine/Util/TimeAnalysis.h"

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
	private:
		void PrintChildren(const TimeRecord& record);
	};
}

