#if HAPTIC_BACKEND == HAPTIC_SDL

#include "SDLCore.h"

#include "Engine/Core/System/Memory.h"
#include "Engine/Util/Log.h"

void InitKeyboard(SDLContext &ctx)
{
    int32 numKeys;
    SDL_GetKeyboardState(&numKeys);
    ctx.rawSDLKeys[0] = wNewArr(bool, numKeys);
    ctx.rawSDLKeys[1] = wNewArr(bool, numKeys);
    for (uint64 i = 0; i < numKeys; i++)
        ctx.rawSDLKeys[0][i] = false;
    for (uint64 i = 0; i < numKeys; i++)
        ctx.rawSDLKeys[1][i] = false;

    ctx.rawKeys[0] = wNewArr(bool, (uint8)WKey::WKey_Count);
    ctx.rawKeys[1] = wNewArr(bool, (uint8)WKey::WKey_Count);
    for (uint64 i = 0; i < (uint8)WKey::WKey_Count; i++)
        ctx.rawKeys[0][i] = false;
    for (uint64 i = 0; i < (uint8)WKey::WKey_Count; i++)
        ctx.rawKeys[1][i] = false;
}

void InitMouse(SDLContext &ctx)
{
    ctx.rawSDLMouse[0] = 0;
    ctx.rawSDLMouse[1] = 0;

    ctx.rawMouse[0] = wNewArr(bool, (uint8)WMouseBtn::WMouseBtn_Count);
    ctx.rawMouse[1] = wNewArr(bool, (uint8)WMouseBtn::WMouseBtn_Count);
    for (uint64 i = 0; i < (uint8)WMouseBtn::WMouseBtn_Count; i++)
        ctx.rawMouse[0][i] = false;
    for (uint64 i = 0; i < (uint8)WMouseBtn::WMouseBtn_Count; i++)
        ctx.rawMouse[1][i] = false;
    ctx.rawMousePos[0] = WEngine::Vector2::Zero;
    ctx.rawMousePos[1] = WEngine::Vector2::Zero;
    ctx.rawMousePosSquare[0] = WEngine::Vector2::Zero;
    ctx.rawMousePosSquare[1] = WEngine::Vector2::Zero;
}

void InitController(SDLContext &ctx)
{
    ctx.rawController[0] = wNewArr(bool, (uint8)WPadBtn::WPadBtn_Count);
    ctx.rawController[1] = wNewArr(bool, (uint8)WPadBtn::WPadBtn_Count);
    for (uint64 i = 0; i < (uint8)WPadBtn::WPadBtn_Count; i++)
        ctx.rawController[0][i] = false;
    for (uint64 i = 0; i < (uint8)WPadBtn::WPadBtn_Count; i++)
        ctx.rawController[1][i] = false;

    ctx.rawLeftJoy[0] = WEngine::Vector2::Zero;
    ctx.rawLeftJoy[1] = WEngine::Vector2::Zero;
    ctx.rawRightJoy[0] = WEngine::Vector2::Zero;
    ctx.rawRightJoy[1] = WEngine::Vector2::Zero;
    ctx.rawLeftTouch[0] = WEngine::Vector2::Zero;
    ctx.rawLeftTouch[1] = WEngine::Vector2::Zero;
    ctx.rawRightTouch[0] = WEngine::Vector2::Zero;
    ctx.rawRightTouch[1] = WEngine::Vector2::Zero;

    ctx.rawLeftTrigger[0] = 0.0f;
    ctx.rawLeftTrigger[1] = 0.0f;
    ctx.rawRightTrigger[0] = 0.0f;
    ctx.rawRightTrigger[1] = 0.0f;
    ctx.rawLeftTouchPressure[0] = 0.0f;
    ctx.rawLeftTouchPressure[1] = 0.0f;
    ctx.rawRightTouchPressure[0] = 0.0f;
    ctx.rawRightTouchPressure[1] = 0.0f;
}

void AdvanceBelts(SDLContext &ctx)
{
    ctx.rawKeys.Cycle();
    ctx.rawMouse.Cycle();
    ctx.rawController.Cycle();

    ctx.rawMousePos.Cycle();
    ctx.rawMousePosSquare.Cycle();
    ctx.rawLeftJoy.Cycle();
    ctx.rawRightJoy.Cycle();
    ctx.rawLeftTouch.Cycle();
    ctx.rawRightTouch.Cycle();
    ctx.rawLeftTrigger.Cycle();
    ctx.rawRightTrigger.Cycle();
    ctx.rawLeftTouchPressure.Cycle();
    ctx.rawRightTouchPressure.Cycle();
}

void CheckControllerStatus(SDLContext &ctx)
{
    if (ctx.isControllerConnected)
        CheckControllerStillConnected(ctx);
    else
        CheckNewControllerConnected(ctx);
}

void CheckControllerStillConnected(SDLContext &ctx)
{
    int32 count;
    SDL_GetGamepads(&count);
    if (count == 0)
    {
        ctx.isControllerConnected = false;
        ctx.controller = nullptr;
        WEngine::WLog::SetConsoleInfo();
        WEngine::WLog::ConsoleLog("Controller was disconnected.");
    }
}

void CheckNewControllerConnected(SDLContext &ctx)
{
    int32 count = 0;
    SDL_JoystickID* controllers = SDL_GetGamepads(&count);
    if (count == 0)
        return;

    ctx.controller = SDL_OpenGamepad(controllers[0]);
    ctx.isControllerConnected = true;
    UpdateControllerType(ctx);
}

void UpdateControllerType(SDLContext &ctx)
{
    auto type = SDL_GetGamepadType(ctx.controller);
    switch (type)
    {
        case SDL_GAMEPAD_TYPE_UNKNOWN:
        case SDL_GAMEPAD_TYPE_STANDARD:
        case SDL_GAMEPAD_TYPE_XBOX360:
        case SDL_GAMEPAD_TYPE_PS3:
        case SDL_GAMEPAD_TYPE_GAMECUBE:
            WEngine::WLog::SetConsoleInfo();
            WEngine::WLog::ConsoleLog("Connected Generic Controller");
            ctx.controllerType = WEngine::ControllerType::Generic;
            break;
        case SDL_GAMEPAD_TYPE_XBOXONE:
            WEngine::WLog::SetConsoleInfo();
            WEngine::WLog::ConsoleLog("Connected Xbox Controller");
            ctx.controllerType = WEngine::ControllerType::Xbox;
            break;
        case SDL_GAMEPAD_TYPE_PS4:
        case SDL_GAMEPAD_TYPE_PS5:
            WEngine::WLog::SetConsoleInfo();
            WEngine::WLog::ConsoleLog("Connected PlayStation Controller");
            ctx.controllerType = WEngine::ControllerType::PlayStation;
            break;
        case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO:
        case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_PAIR:
            WEngine::WLog::SetConsoleInfo();
            WEngine::WLog::ConsoleLog("Connected Nintendo Switch Controller");
            ctx.controllerType = WEngine::ControllerType::Nintendo;
            break;
        case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_LEFT:
        case SDL_GAMEPAD_TYPE_STEAM:
            WEngine::WLog::SetConsoleInfo();
            WEngine::WLog::ConsoleLog("Connected Valve Controller");
            ctx.controllerType = WEngine::ControllerType::Valve;
            break;
        case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT:
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Please connect both Joy Cons.");
            ctx.controllerType = WEngine::ControllerType::Generic;
            break;
        default: ;
    }
}

#endif
