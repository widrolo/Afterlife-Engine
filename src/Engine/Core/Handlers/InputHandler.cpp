#include "InputHandler.h"

#include <Engine/Types/Input/KeyEvents.h>
#include <Engine/Util/Log.h>
#include <Engine/Util/BitwiseMacros.h>
#include <Engine/Types/CoreSystems.h>
#include <Engine/EngineDefines.h>

#include <Engine/imgui/imgui.h>
#include <Engine/imgui/backends/imgui_impl_sdl3.h>
#include <Engine/Core/System/Memory.h>

#include "Editor/Types/EditorSystems.h"

#include <steam_api.h>

using namespace WEngine;

float Sint16ToFloat(const int16 num)
{
	float normalized;
	if (num < 0)
		normalized = (float)num / 32768.0f;
	else
		normalized = (float)num / 32767.0f;
	return normalized;
}

// hardest thing is programming is naming things
// this take a -1 to 1 float and makes it 0 to 1
float FloatRange1_1To0_1(const float num)
{
	return (num + 1.0f) / 2.0f;
}

InputHandler::InputHandler()
{
	int numkeys;
	SDL_GetKeyboardState(&numkeys);
	m_keystate = (bool*)wNew(numkeys);
	m_lastKeystate = (bool*)wNew(numkeys);
	m_controllers = wNewArr(InputHandle_t, STEAM_INPUT_MAX_COUNT);

	// prevents random key presses on boot

	for (int i = 0; i < numkeys; i++)
	{
		m_keystate[i] = false;
	}
	for (int i = 0; i < numkeys; i++)
	{
		m_lastKeystate[i] = false;
	}
}

void InputHandler::SetWindow(SDL_Window *window)
{
	m_window = window;
}

void InputHandler::FetchInput()
{
	FetchKeyboardInput();
	FetchMouseInput();

	FetchControllerInput();
	
	PollEvents();
}

bool InputHandler::GetKeyPressed(WKey key) const
{
	return (m_keystate[static_cast<int>(key)] && !m_lastKeystate[static_cast<int>(key)]);
}

bool InputHandler::GetKeyHold(WKey key) const
{
	return (m_keystate[static_cast<int>(key)]);
}

bool InputHandler::GetKeyReleased(WKey key) const
{
	return (!m_keystate[static_cast<int>(key)] && m_lastKeystate[static_cast<int>(key)]);
}

Vector2 InputHandler::GetMousePos() const
{
	return m_mousePosition;
}

void InputHandler::SetMouseRelativeMode(bool mode)
{
	SDL_SetWindowRelativeMouseMode(m_window, mode);
	if (!mode)
		SDL_ShowCursor();
}

void InputHandler::SetMousePos(const Vector2 &pos)
{
	m_mousePosition = pos;
	SDL_WarpMouseInWindow(m_window, pos.x, pos.y);
	//SDL_FlushEvent(SDL_EVENT_MOUSE_MOTION);
}

bool InputHandler::GetMousePressed(WMouseBtn button) const
{
	return ((m_mousestate & BIT(static_cast<int>(button))) &&
		!(m_lastMousestate & BIT(static_cast<int>(button))));
}

bool InputHandler::GetMouseHold(WMouseBtn button) const
{
	return (m_mousestate & BIT(static_cast<int>(button)));
}

bool InputHandler::GetMouseReleased(WMouseBtn button) const
{
	return (!(m_mousestate & BIT(static_cast<int>(button))) &&
		(m_lastMousestate & BIT(static_cast<int>(button))));
}

void InputHandler::OpenConfigPanel()
{
	if (!m_isControllerConnected)
		return;
	SteamInput()->ShowBindingPanel(m_mainController);
}

void InputHandler::FetchKeyboardInput()
{
	// Swap keystate buffers
	bool* temp = m_lastKeystate;
	m_lastKeystate = m_keystate;
	m_keystate = temp;

	// Update keystates
	int numkeys;
	bool* sdlkeyState = const_cast<bool*>(SDL_GetKeyboardState(&numkeys));
	std::memcpy(m_keystate, sdlkeyState, sizeof(bool) * numkeys);
}

void InputHandler::FetchMouseInput()
{
	// It's ok, since im not swapping buffer unlike above
	m_lastMousestate = m_mousestate;

	float32 mouseY = 0.0f, mouseX = 0.0f;

	if (SDL_GetWindowRelativeMouseMode(m_window))
		m_mousestate = SDL_GetRelativeMouseState(&mouseX, &mouseY);
	else
		m_mousestate = SDL_GetMouseState(&mouseX, &mouseY);
	m_mousePosition = { mouseX, mouseY };
}

void InputHandler::FetchControllerInput()
{
	static InputActionSetHandle_t actionSet = SteamInput()->GetActionSetHandle("General");
	static InputDigitalActionHandle_t testAction = SteamInput()->GetDigitalActionHandle("Test");

	SteamInput()->RunFrame();
	m_numControllers = SteamInput()->GetConnectedControllers(m_controllers);
	if (m_numControllers != 0 && m_controllers != nullptr)
	{
		m_mainController = m_controllers[0];
		m_isControllerConnected = true;

		SteamInput()->ActivateActionSet(m_mainController, actionSet);

		auto ret = SteamInput()->GetDigitalActionData(m_mainController, testAction);
		if (ret.bState)
			WLog::ConsoleLog("Test is true!!");
	}
	else
	{
		m_isControllerConnected = false;
	}
}


void InputHandler::PollEvents() const
{
	while (SDL_PollEvent(m_inputEvent))
	{
		ImGui_ImplSDL3_ProcessEvent(m_inputEvent);
		if (m_inputEvent->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
		{
			if (!m_isEditor)
				CoreSystems::ShutdownGame();
			else
				WEditor::EditorSystems::ShutdownEditor();
		}
	}
}
