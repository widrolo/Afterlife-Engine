#pragma once
#if HAPTIC_BACKEND == HAPTIC_SDL
#include <string>
#include <unordered_map>
#include <vector>

#include "Engine/Types/Input/InputDevice.h"
#include "Engine/Types/Input/KeyCodes.h"

// since SDL uses underscores for its own API, we have to use no underscores
// for our types here to avoid collisions.

struct SDLSense
{
    bool keyboardCheck : 1;
    bool controllerCheck : 1;
    bool mouseCheck : 1;

    WEngine::InputSenseKind inputKind : 2;
    WKey key;
    WMouseBtn mouse;
    WPadBtn controller;
    std::string name;
};

struct FetchResult
{
    WEngine::Vector2 pos;
    float32 trigger;
    bool justPressed : 1;
    bool held : 1;
    bool justReleased : 1;
};

struct SDLMap
{
    std::string name;
    std::vector<SDLSense> senses;
    std::unordered_map<std::string, FetchResult> results;
};

#endif