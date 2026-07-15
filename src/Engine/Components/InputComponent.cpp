#include "InputComponent.h"

#include <Engine/Core/Handlers/InputHandler.h>
#include <Engine/Types/CoreSystems.h>
#include <Engine/Core/World/Entity.h>

using namespace WEngine;

REGISTER_COMPONENT(InputComponent)

InputComponent::InputComponent(Entity* e)
{
	COMP_SETUP("InputHandler")
}

bool InputComponent::GetActionInput(const WKey key)
{
	return CoreSystems::GetInputHandler()->GetKeyHold(key);
}

bool InputComponent::GetActionInput(const WKey key, const PressType pressType)
{
	switch (pressType)
	{
	case Press:
		return CoreSystems::GetInputHandler()->GetKeyPressed(key);
	case Hold:
		return CoreSystems::GetInputHandler()->GetKeyHold(key);
	case Release:
		return CoreSystems::GetInputHandler()->GetKeyReleased(key);
	default:
		return false;
	}
}

bool InputComponent::GetActionInput(const WMouseBtn button)
{
	return CoreSystems::GetInputHandler()->GetMouseHold(button);
}

bool InputComponent::GetActionInput(const WMouseBtn button, const PressType pressType)
{
	switch (pressType)
	{
	case Press:
		return CoreSystems::GetInputHandler()->GetMousePressed(button);
	case Hold:
		return CoreSystems::GetInputHandler()->GetMouseHold(button);
	case Release:
		return CoreSystems::GetInputHandler()->GetMouseReleased(button);
	default:
		return false;
	}
}

Vector2 InputComponent::GetMousePosition()
{
	return CoreSystems::GetInputHandler()->GetMousePos();
}