#if HAPTIC_BACKEND == HAPTIC_SDL

#include "SDLFetching.h"

#include <cstring>

void FetchAllInput(SDLContext &ctx)
{
    FetchKeyboard(ctx);
    FetchMouse(ctx);
    FetchController(ctx);
}

void FetchKeyboard(SDLContext &ctx)
{
    int32 numKeys;
    const auto sdlKeyState = SDL_GetKeyboardState(&numKeys);
    std::memcpy(ctx.rawSDLKeys[0], sdlKeyState, numKeys);
}

void FetchMouse(SDLContext &ctx)
{
    float32 mouseY = 0.0f, mouseX = 0.0f;
    if (SDL_GetWindowRelativeMouseMode(ctx.window))
        ctx.rawSDLMouse[0] = SDL_GetRelativeMouseState(&mouseX, &mouseY);
    else
        ctx.rawSDLMouse[0] = SDL_GetMouseState(&mouseX, &mouseY);
    ctx.rawMousePos[0] = { mouseX, mouseY };
}

void FetchController(SDLContext &ctx)
{
    if (!ctx.isControllerConnected)
        return;
}

void UpdateAllSenses(SDLContext &ctx)
{
    for (auto& map : ctx.maps)
        UpdateAllSensesInMap(ctx, map);
}

void UpdateAllSensesInMap(SDLContext &ctx, SDLMap &map)
{
    for (auto& sense : map.senses)
    {

    }
}

#endif
