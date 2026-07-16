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

    for (sizeT i = 0; i < SDL_GAMEPAD_BUTTON_COUNT; i++)
        ctx.rawSDLController[0][i] = SDL_GetGamepadButton(ctx.controller, (SDL_GamepadButton)i);

}

void TranslateFetched(SDLContext &ctx)
{
    TranslateKeyboard(ctx);
    TranslateMouse(ctx);
    TranslateController(ctx);
}

void TranslateKeyboard(SDLContext &ctx)
{
    // Absolutely Unreadable

    // WKey -> 0...9
    // SDL  -> 1...9 and 0
    // numbers
    ctx.rawKeys[0][(sizeT)WKey::N0] = ctx.rawSDLKeys[0][SDL_SCANCODE_0];
    for (sizeT i = (sizeT)WKey::N1; i <= (sizeT)WKey::N9; i++)
        ctx.rawKeys[0][i] = ctx.rawSDLKeys[0][i - (sizeT)WKey::N1 + SDL_SCANCODE_1];

    // Idk how much of a guarantee it is that scancode order stay
    // so lets play it safe.

    // special
    ctx.rawKeys[0][(sizeT)WKey::ENTER] = ctx.rawSDLKeys[0][SDL_SCANCODE_RETURN];
    ctx.rawKeys[0][(sizeT)WKey::ESCAPE] = ctx.rawSDLKeys[0][SDL_SCANCODE_ESCAPE];
    ctx.rawKeys[0][(sizeT)WKey::BACKSPACE] = ctx.rawSDLKeys[0][SDL_SCANCODE_BACKSPACE];
    ctx.rawKeys[0][(sizeT)WKey::TAB] = ctx.rawSDLKeys[0][SDL_SCANCODE_TAB];
    ctx.rawKeys[0][(sizeT)WKey::SPACE] = ctx.rawSDLKeys[0][SDL_SCANCODE_SPACE];
    ctx.rawKeys[0][(sizeT)WKey::CAPSLOCK] = ctx.rawSDLKeys[0][SDL_SCANCODE_CAPSLOCK];
    ctx.rawKeys[0][(sizeT)WKey::L_SHIFT] = ctx.rawSDLKeys[0][SDL_SCANCODE_LSHIFT];
    ctx.rawKeys[0][(sizeT)WKey::L_CONTROL] = ctx.rawSDLKeys[0][SDL_SCANCODE_LCTRL];
    ctx.rawKeys[0][(sizeT)WKey::L_ALT] = ctx.rawSDLKeys[0][SDL_SCANCODE_LALT];
    ctx.rawKeys[0][(sizeT)WKey::R_SHIFT] = ctx.rawSDLKeys[0][SDL_SCANCODE_RSHIFT];
    ctx.rawKeys[0][(sizeT)WKey::R_CONTROL] = ctx.rawSDLKeys[0][SDL_SCANCODE_RCTRL];
    ctx.rawKeys[0][(sizeT)WKey::R_ALT] = ctx.rawSDLKeys[0][SDL_SCANCODE_RALT];

    // arrows
    ctx.rawKeys[0][(sizeT)WKey::LEFT] = ctx.rawSDLKeys[0][SDL_SCANCODE_LEFT];
    ctx.rawKeys[0][(sizeT)WKey::RIGHT] = ctx.rawSDLKeys[0][SDL_SCANCODE_RIGHT];
    ctx.rawKeys[0][(sizeT)WKey::UP] = ctx.rawSDLKeys[0][SDL_SCANCODE_UP];
    ctx.rawKeys[0][(sizeT)WKey::DOWN] = ctx.rawSDLKeys[0][SDL_SCANCODE_DOWN];

    // keys
    for (sizeT i = (sizeT)WKey::A; i <= (sizeT)WKey::Z; i++)
        ctx.rawKeys[0][i] = ctx.rawSDLKeys[0][i - (sizeT)WKey::A + SDL_SCANCODE_A];

    // debug
    for (sizeT i = (sizeT)WKey::DEBUG1; i <= (sizeT)WKey::DEBUG12; i++)
        ctx.rawKeys[0][i] = ctx.rawSDLKeys[0][i - (sizeT)WKey::DEBUG1 + SDL_SCANCODE_F1];
}

void TranslateMouse(SDLContext &ctx)
{

}

void TranslateController(SDLContext &ctx)
{
    if (!ctx.isControllerConnected)
        return;

    // Its not exactly that trivial to just loop, and i feel a bit safer with this approach
    ctx.rawController[0][(sizeT)WPadBtn::South]     = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_SOUTH];
    ctx.rawController[0][(sizeT)WPadBtn::East]      = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_EAST];
    ctx.rawController[0][(sizeT)WPadBtn::West]      = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_WEST];
    ctx.rawController[0][(sizeT)WPadBtn::North]     = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_NORTH];
    ctx.rawController[0][(sizeT)WPadBtn::Menu1]     = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_START];
    ctx.rawController[0][(sizeT)WPadBtn::Menu2]     = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_BACK];
    ctx.rawController[0][(sizeT)WPadBtn::Menu3]     = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_GUIDE];
    ctx.rawController[0][(sizeT)WPadBtn::LS]        = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_LEFT_STICK];
    ctx.rawController[0][(sizeT)WPadBtn::RS]        = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_RIGHT_STICK];
    ctx.rawController[0][(sizeT)WPadBtn::LB]        = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_LEFT_SHOULDER];
    ctx.rawController[0][(sizeT)WPadBtn::RB]        = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER];
    ctx.rawController[0][(sizeT)WPadBtn::Up]        = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_DPAD_UP];
    ctx.rawController[0][(sizeT)WPadBtn::Down]      = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_DPAD_DOWN];
    ctx.rawController[0][(sizeT)WPadBtn::Left]      = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_DPAD_LEFT];
    ctx.rawController[0][(sizeT)WPadBtn::Right]     = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_DPAD_RIGHT];
    ctx.rawController[0][(sizeT)WPadBtn::L4]        = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_LEFT_PADDLE1];
    ctx.rawController[0][(sizeT)WPadBtn::R4]        = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1];
    ctx.rawController[0][(sizeT)WPadBtn::L5]        = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_LEFT_PADDLE2];
    ctx.rawController[0][(sizeT)WPadBtn::R5]        = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2];

    if (ctx.controllerFeatures & (word)WEngine::ControllerFeatures::Touch_Click_Remap)
        ctx.rawController[0][(sizeT)WPadBtn::Menu2] = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_TOUCHPAD];
    else
        ctx.rawController[0][(sizeT)WPadBtn::Touchpad] = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_TOUCHPAD];

    // Haptic doesnt read triggers yet.
    //ctx.rawController[0][(sizeT)WPadBtn::LT] = ctx.rawSDLController[0][];
    //ctx.rawController[0][(sizeT)WPadBtn::RT] = ctx.rawSDLController[0][];
}

void UpdateAllSenses(SDLContext &ctx)
{
    for (auto& map : ctx.maps)
        UpdateAllSensesInMap(ctx, map);
}

void UpdateAllSensesInMap(SDLContext &ctx, SDLMap &map)
{
    for (const auto& sense : map.senses)
    {
        bool justPressed = false;
        bool pressed = false;
        bool justReleased = false;

        bool canPossiblyJP = true;
        bool wasDownLastFrame = false;
        if (sense.keyboardCheck)
        {
            if (ctx.rawKeys[0][(sizeT)sense.key] && !ctx.rawKeys[1][(sizeT)sense.key])
                justPressed = true;
            if (ctx.rawKeys[0][(sizeT)sense.key])
                pressed = true;
            if (!ctx.rawKeys[0][(sizeT)sense.key] && ctx.rawKeys[1][(sizeT)sense.key])
                justReleased = true;

            if (ctx.rawKeys[0][(sizeT)sense.key] && ctx.rawKeys[1][(sizeT)sense.key])
                canPossiblyJP = false;
            if (ctx.rawKeys[1][(uint8)sense.key])
                wasDownLastFrame = true;
        }
        if (sense.controllerCheck)
        {
            if (ctx.rawController[0][(sizeT)sense.controller] && !ctx.rawController[1][(sizeT)sense.controller])
                justPressed = true;
            if (ctx.rawController[0][(sizeT)sense.controller])
                pressed = true;
            if (!ctx.rawController[0][(sizeT)sense.controller] && ctx.rawController[1][(sizeT)sense.controller])
                justReleased = true;

            if (ctx.rawController[0][(sizeT)sense.controller] && ctx.rawController[1][(sizeT)sense.controller])
                canPossiblyJP = false;
            if (ctx.rawKeys[1][(uint8)sense.key])
                wasDownLastFrame = true;
        }
        FetchResult fetch{};
        if (canPossiblyJP && !wasDownLastFrame) fetch.justPressed = justPressed;
        fetch.held = pressed;
        if (!pressed) fetch.justReleased = justReleased;
        map.results[sense.name] = fetch;
    }
}

#endif
