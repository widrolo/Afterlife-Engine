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

		wtl::vector<InputPeripheral> m_peripherals;

		SDL_Window* m_window;

		bool m_isEditor = false;
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

		/**
		 * Gets the position of the left joystick.
		 * @param controller Controller to be queried.
		 * @return Left joystick position.
		 */
		[[nodiscard]] Vector2 GetLeftStick(const Controller &controller) const;

		/**
		 * Gets the position of the right joystick.
		 * @param controller Controller to be queried.
		 * @return Right joystick position.
		 */
		[[nodiscard]] Vector2 GetRightStick(const Controller &controller) const;

		/**
		 * Gets how far the left trigger is pressed down.
		 * @param controller Controller to be queried.
		 * @return Depth of the left trigger.
		 */
		[[nodiscard]] float GetLeftTrigger(const Controller &controller) const;

		/**
		 * Gets how far the right trigger is pressed down.
		 * @param controller Controller to be queried.
		 * @return Depth of the right trigger.
		 */
		[[nodiscard]] float GetRightTrigger(const Controller &controller) const;

		/**
		 * @deprecated
		 */
		[[deprecated]][[nodiscard]] Touchpad GetTouchPad() const;

		/**
		 * Checks whether a controller is connected.
		 * @return True if a controller is connected.
		 */
		[[nodiscard]] bool GetControllerPresent() const;

		/**
		 * Checks if a mouse button was newly pressed this frame.
		 * @param buttonMap Button map of the controller to be queried.
		 * @param button The gamepad button to be queried for a press.
		 * @return True if the mouse button was pressed this frame.
		 */
		[[nodiscard]] bool GetGamepadPressed(InputPeripheralButtons buttonMap, WPadBtn button) const;

		/**
		 * Checks if a gamepad button is being pressed down.
		 * @param buttonMap Button map of the controller to be queried.
		 * @param button The gamepad button to be queried for a press.
		 * @return True if the gamepad button is currently in a pressed state.
		 */
		[[nodiscard]] bool GetGamepadHold(InputPeripheralButtons buttonMap, WPadBtn button) const;

		/**
		 * Checks if a gamepad button has just been released this frame.
		 * @param buttonMap Button map of the controller to be queried.
		 * @param button The gamepad button to be queried for a press.
		 * @return True if the gamepad button was just released this frame.
		 */
		[[nodiscard]] bool GetGamepadReleased(InputPeripheralButtons buttonMap, WPadBtn button) const;

		/**
		 * Gets an input peripheral from the repository.
		 * @param id ID of a connected peripheral to be retrieved.
		 * @return The retrieved input peripheral.
		 */
		[[nodiscard]] const InputPeripheral& GetInputPeripheral(uint8 id) const;

		/**
		 * Gets the number of connected peripherals.
		 * @return Number of connected peripherals.
		 */
		[[nodiscard]] uint8 GetInputPeripheralCount() const;

		/**
		 * Checks whether a peripheral is still connected.
		 * @param joyID ID of the peripheral.
		 * @return True if It's still connected.
		 */
		[[nodiscard]] bool IsInputPeripheralStillPresent(const SDL_JoystickID* joyID) const;
	private:
		void InitPeripheral(SDL_JoystickID joyID);
		static Controller* InitController(SDL_Joystick* joystick);
		static SteeringWheel* InitSteeringWheel(SDL_Joystick* joystick);
		static InputPeripheralVendor GetVendor(uint16 id) ;

		void FetchKeyboardInput();
		void FetchMouseInput();
		void FetchControllerInput();
		void FetchSteeringWheelInput();
		static void FetchPadButtonInput(InputPeripheral& device);

		// these are not all, since i dont have them all
		static void FetchPadButtonInput_Sony(const InputPeripheral& device, const bool* joyButtons);
		static void FetchPadButtonInput_Microsoft(const InputPeripheral& device, const bool* joyButtons);
		static void FetchPadButtonInput_Logitech(const InputPeripheral& device, const bool* joyButtons);
		static void FetchPadButtonInput_Turtlebeach(const InputPeripheral& device, const bool* joyButtons);

		void CheckPeripherals();
		void CheckMissingPeripherals(int count, const SDL_JoystickID* joyID);
		void PollEvents() const;

		static void ResetController(Controller* controller);
		static void ResetSteeringWheel(SteeringWheel* wheel);
	};
}
