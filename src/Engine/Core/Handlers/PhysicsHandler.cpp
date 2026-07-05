#include "PhysicsHandler.h"

#include <Engine/EngineDefines.h>

#include <Engine/Util/Log.h>
#include <Engine/Types/CoreSystems.h>
#include <Engine/Core/Handlers/RenderHandler.h>

#include "Editor/Types/EditorState.h"

using namespace WEngine;

PhysicsHandler::PhysicsHandler()
{
	if constexpr (!PhysicsSettings::physicsEnabled)
		return;


}

void PhysicsHandler::Tick()
{
	if constexpr (!PhysicsSettings::physicsEnabled)
		return;

}

uint64 PhysicsHandler::MakeSimulatableObject()
{
	if (WEditor::EditorState::EditorMode)
		return 0;
	if constexpr (!PhysicsSettings::physicsEnabled)
		return 0;

	return {};
}

Nullable<SimulatableObject*> PhysicsHandler::GetSimulatableObject(uint64 id)
{
	if (WEditor::EditorState::EditorMode)
		return Nullable<SimulatableObject*>();
	if constexpr (!PhysicsSettings::physicsEnabled)
		return Nullable<SimulatableObject*>();

	return {};
}

void PhysicsHandler::DeleteSimulatableObject(uint64 id)
{
	if constexpr (!PhysicsSettings::physicsEnabled)
		return;

}

wtl::vector<OverlapResult> PhysicsHandler::CheckOverlapping(uint64 id)
{
	if constexpr (!PhysicsSettings::physicsEnabled)
		return wtl::vector<OverlapResult>();

	return {};
}


void PhysicsHandler::Setup()
{
	if constexpr (!PhysicsSettings::physicsEnabled)
		return;
}


void PhysicsHandler::Visualize()
{
	if constexpr (!PhysicsSettings::physicsVisualize)
		return;

}
