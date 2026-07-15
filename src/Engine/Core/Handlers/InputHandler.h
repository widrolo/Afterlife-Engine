#pragma once

// This file does not contain LLM generated documentation

#include <SDL3/SDL.h>
#include <cstring>
#include <algorithm>
#include <Engine/WTL/vector.h>

#include <Engine/Math/Vector.h>
#include <Engine/Types/Input/KeyCodes.h>
#include <Engine/Types/CommonTypes.h>
#include <Engine/Types/Input/InputDevice.h>

#include <steam_api.h>

namespace WEngine
{
	/**
	 * This handler is responsible for reading all available input each frame and making it available to the programmer in a nice interface.
	 */
	class InputHandler
	{
	public:
		InputHandler();
	private:
		SDL_Event* m_inputEvent = new SDL_Event;
		bool* m_keystate;
		bool* m_lastKeystate;

		Vector2 m_mousePosition;
		uint32 m_mousestate{};
		uint32 m_lastMousestate{};

		SDL_Window* m_window;

		bool m_isEditor = false;

		InputHandle_t* m_controllers = nullptr;
		uint8 m_numControllers;
		bool m_isControllerConnected;
		InputHandle_t m_mainController;

	public:

		void EnableEditorMode() { m_isEditor = true; }

		/**
		 * Sets the primary window for catching input.
		 * @param window The window from which the handler will read input.
		 */
		void SetWindow(SDL_Window* window);

		/**
		 * Fetches all input.
		 * @warning This should be called by the game loop only.
		 */
		void FetchInput();

		/**
		 * Checks if a key was newly pressed this frame.
		 * @param key The key to be queried for a press.
		 * @return True if the key was pressed this frame.
		 */
		[[nodiscard]] bool GetKeyPressed(WKey key) const;

		/**
		 * Checks if a key is being pressed down.
		 * @param key The key to be queried for a press.
		 * @return True if the key is currently in a pressed state.
		 */
		[[nodiscard]] bool GetKeyHold(WKey key) const;

		/**
		 * Checks if a key has just been released this frame.
		 * @param key The key to be queried for a press.
		 * @return True if the key was just released this frame.
		 */
		[[nodiscard]] bool GetKeyReleased(WKey key) const;

		/**
		 * Gets the position of the mouse in pixels.
		 * @return Pixel position of the mouse.
		 */
		[[nodiscard]] Vector2 GetMousePos() const;

		/**
		 * Basically, it captures the mouse withing the window.
		 * @param mode Enabled if true.
		 */
		void SetMouseRelativeMode(bool mode);

		/**
		 * Set the position of the mouse to a specific pixel.
		 * @param pos Pixel position of the mouse.
		 */
		void SetMousePos(const Vector2& pos);

		/**
		 * Checks if a mouse button was newly pressed this frame.
		 * @param button The mouse button to be queried for a press.
		 * @return True if the mouse button was pressed this frame.
		 */
		[[nodiscard]] bool GetMousePressed(WMouseBtn button) const;

		/**
		 * Checks if a mouse button is being pressed down.
		 * @param button The mouse button to be queried for a press.
		 * @return True if the mouse button is currently in a pressed state.
		 */
		[[nodiscard]] bool GetMouseHold(WMouseBtn button) const;

		/**
		 * Checks if a mouse button has just been released this frame.
		 * @param button The mouse button to be queried for a press.
		 * @return True if the mouse button was just released this frame.
		 */
		[[nodiscard]] bool GetMouseReleased(WMouseBtn button) const;

		void OpenConfigPanel();

	private:
		void FetchKeyboardInput();
		void FetchMouseInput();
		void FetchControllerInput();


		void PollEvents() const;

	};
}
