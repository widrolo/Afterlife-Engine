#include "Sector.h"

#include <string>

#include "Engine/Core/Handlers/RenderHandler.h"
#include "Engine/Types/CoreSystems.h"

using namespace WEngine;

void Sector::Draw()
{
	CoreSystems::GetRenderHandler()->AddPlanToRenderQueue(m_renderPlan);
}
