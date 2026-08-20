#pragma once

#include <Engine/Types/Rendering/Color.h>
#include <Engine/Math/Shapes.h>
#include <Engine/Types/CommonTypes.h>

#include "ShaderSettings.h"
#include "Engine/Math/Transform.h"
#include "Iris/Handles.h"

namespace WEngine
{
	/**
	 * Determines the order in which the layers will be rendered.
	 */
	enum class RenderLayer : uint8
	{
		None = 0, ///< Not rendered.
		Default = 1, ///< Default.
		UI = 255, ///< UI, should be the top most in gameplay.
	};

	struct RenderMission
	{
		uint64 textureUID;
		uint64 meshUID;
		Transform transform;
	};

	struct RenderPlanPart
	{
		// yes its less then 64 bit, i know. But there is no way were going
		// to have more than 4 billion meshes anyway.
		uint32 textureUID;
		uint32 meshUID;
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
