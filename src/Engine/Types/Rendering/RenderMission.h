#pragma once

#include <Engine/Types/Rendering/Color.h>
#include <Engine/Math/Shapes.h>
#include <Engine/Types/CommonTypes.h>

#include "ShaderSettings.h"
#include "Engine/Math/Transform.h"
#include "Engine/Util/BitwiseMacros.h"
#include "Iris/Handles.h"

namespace WEngine
{
	enum class RenderMissionFlags
	{
		PhysicsDynamic = BIT(0), // Makes the object the dynamic color in render debug view
		PhysicsSleeping = BIT(1), // Makes the object the sleep color in render debug view; takes priority over dynamic
	};
	DEFINE_ENUM_BITWISE(RenderMissionFlags)

	struct RenderMission
	{
		uint64 textureUID;
		uint64 meshUID;
		uint64 phyMeshUID = 0; // not really an obligation but pls put it in there as well for the sake of debugging
		Transform transform;
		RenderMissionFlags flags;
	};

	struct RenderPlanPart
	{
		// yes its less then 64 bit, i know. But there is no way were going
		// to have more than 4 billion meshes anyway.
		uint32 textureUID;
		uint32 meshUID;
		uint32 phyMeshUID = 0;
		uint16 offset; // in number of items, not bytes
		uint16 count;  // in number of items, not bytes
	};

	struct RenderPlan
	{
		wtl::vector<RenderPlanPart> parts;
		Iris::BufferHandle statBuffer;
	};

	struct RenderVisualizationMission
	{
		Color color;
		wtl::vector<Line2D> lines;
	};
}
