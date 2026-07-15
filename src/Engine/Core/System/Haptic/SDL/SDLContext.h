#pragma once
#if HAPTIC_BACKEND == HAPTIC_SDL

#include <Engine/Types/Input/InputSense.h>

#include "SDLTypes.h"
#include "Engine/Types/ValueBelt.h"

struct SDLContext
{
    std::vector<SDLMap> maps;
    SDLMap* selectedMap;
    SDL_Window* window;
    SDL_Event event;
    bool isEditor;

    WEngine::ValueBelt<bool*, 2> rawKeys;
    WEngine::ValueBelt<bool*, 2> rawSDLKeys;

    WEngine::ValueBelt<bool*, 2> rawMouse;
    WEngine::ValueBelt<uint32, 2> rawSDLMouse;
    WEngine::ValueBelt<WEngine::Vector2, 2> rawMousePos;
    WEngine::ValueBelt<WEngine::Vector2, 2> rawMousePosSquare;

    WEngine::ValueBelt<bool*, 2> rawController;
    WEngine::ValueBelt<bool*, 2> rawSDLController;
    WEngine::ValueBelt<WEngine::Vector2, 2> rawLeftJoy;
    WEngine::ValueBelt<WEngine::Vector2, 2> rawRightJoy;
    WEngine::ValueBelt<WEngine::Vector2, 2> rawLeftTouch;
    WEngine::ValueBelt<WEngine::Vector2, 2> rawRightTouch;
    WEngine::ValueBelt<float32, 2> rawLeftTrigger;
    WEngine::ValueBelt<float32, 2> rawRightTrigger;
    WEngine::ValueBelt<float32, 2> rawLeftTouchPressure;
    WEngine::ValueBelt<float32, 2> rawRightTouchPressure;

    bool isControllerConnected = false;
    SDL_Gamepad* controller = nullptr;
    WEngine::ControllerType controllerType;
    uint16 controllerFeatures = 0;
};

#endif

