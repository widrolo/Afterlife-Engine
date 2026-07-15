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

    for (uint64 i = 0; i < SDL_GAMEPAD_BUTTON_COUNT; i++)
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
    ctx.rawKeys[0][(uint8)WKey::N0] = ctx.rawSDLKeys[0][SDL_SCANCODE_0];
    for (uint64 i = (uint8)WKey::N1; i <= (uint8)WKey::N9; i++)
        ctx.rawKeys[0][i] = ctx.rawSDLKeys[0][i - (uint8)WKey::N1 + SDL_SCANCODE_1];

    // Idk how much of a guarantee it is that scancode order stay
    // so lets play it safe.

    // special
    ctx.rawKeys[0][(uint8)WKey::ENTER] = ctx.rawSDLKeys[0][SDL_SCANCODE_RETURN];
    ctx.rawKeys[0][(uint8)WKey::ESCAPE] = ctx.rawSDLKeys[0][SDL_SCANCODE_ESCAPE];
    ctx.rawKeys[0][(uint8)WKey::BACKSPACE] = ctx.rawSDLKeys[0][SDL_SCANCODE_BACKSPACE];
    ctx.rawKeys[0][(uint8)WKey::TAB] = ctx.rawSDLKeys[0][SDL_SCANCODE_TAB];
    ctx.rawKeys[0][(uint8)WKey::SPACE] = ctx.rawSDLKeys[0][SDL_SCANCODE_SPACE];
    ctx.rawKeys[0][(uint8)WKey::CAPSLOCK] = ctx.rawSDLKeys[0][SDL_SCANCODE_CAPSLOCK];
    ctx.rawKeys[0][(uint8)WKey::L_SHIFT] = ctx.rawSDLKeys[0][SDL_SCANCODE_LSHIFT];
    ctx.rawKeys[0][(uint8)WKey::L_CONTROL] = ctx.rawSDLKeys[0][SDL_SCANCODE_LCTRL];
    ctx.rawKeys[0][(uint8)WKey::L_ALT] = ctx.rawSDLKeys[0][SDL_SCANCODE_LALT];
    ctx.rawKeys[0][(uint8)WKey::R_SHIFT] = ctx.rawSDLKeys[0][SDL_SCANCODE_RSHIFT];
    ctx.rawKeys[0][(uint8)WKey::R_CONTROL] = ctx.rawSDLKeys[0][SDL_SCANCODE_RCTRL];
    ctx.rawKeys[0][(uint8)WKey::R_ALT] = ctx.rawSDLKeys[0][SDL_SCANCODE_RALT];

    // arrows
    ctx.rawKeys[0][(uint8)WKey::LEFT] = ctx.rawSDLKeys[0][SDL_SCANCODE_LEFT];
    ctx.rawKeys[0][(uint8)WKey::RIGHT] = ctx.rawSDLKeys[0][SDL_SCANCODE_RIGHT];
    ctx.rawKeys[0][(uint8)WKey::UP] = ctx.rawSDLKeys[0][SDL_SCANCODE_UP];
    ctx.rawKeys[0][(uint8)WKey::DOWN] = ctx.rawSDLKeys[0][SDL_SCANCODE_DOWN];

    // keys
    for (uint64 i = (uint8)WKey::A; i <= (uint8)WKey::Z; i++)
        ctx.rawKeys[0][i] = ctx.rawSDLKeys[0][i - (uint8)WKey::A + SDL_SCANCODE_A];

    // debug
    for (uint64 i = (uint8)WKey::DEBUG1; i <= (uint8)WKey::DEBUG12; i++)
        ctx.rawKeys[0][i] = ctx.rawSDLKeys[0][i - (uint8)WKey::DEBUG1 + SDL_SCANCODE_F1];
}

void TranslateMouse(SDLContext &ctx)
{

}

void TranslateController(SDLContext &ctx)
{
    if (!ctx.isControllerConnected)
        return;

    // Its not exactly that trivial to just loop, and i feel a bit safer with this approach
    ctx.rawController[0][(uint8)WPadBtn::South]     = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_SOUTH];
    ctx.rawController[0][(uint8)WPadBtn::East]      = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_EAST];
    ctx.rawController[0][(uint8)WPadBtn::West]      = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_WEST];
    ctx.rawController[0][(uint8)WPadBtn::North]     = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_NORTH];
    ctx.rawController[0][(uint8)WPadBtn::Menu1]     = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_START];
    ctx.rawController[0][(uint8)WPadBtn::Menu2]     = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_BACK];
    ctx.rawController[0][(uint8)WPadBtn::Menu3]     = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_GUIDE];
    ctx.rawController[0][(uint8)WPadBtn::LS]        = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_LEFT_STICK];
    ctx.rawController[0][(uint8)WPadBtn::RS]        = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_RIGHT_STICK];
    ctx.rawController[0][(uint8)WPadBtn::LB]        = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_LEFT_SHOULDER];
    ctx.rawController[0][(uint8)WPadBtn::RB]        = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER];
    ctx.rawController[0][(uint8)WPadBtn::Up]        = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_DPAD_UP];
    ctx.rawController[0][(uint8)WPadBtn::Down]      = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_DPAD_DOWN];
    ctx.rawController[0][(uint8)WPadBtn::Left]      = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_DPAD_LEFT];
    ctx.rawController[0][(uint8)WPadBtn::Right]     = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_DPAD_RIGHT];
    ctx.rawController[0][(uint8)WPadBtn::L4]        = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_LEFT_PADDLE1];
    ctx.rawController[0][(uint8)WPadBtn::R4]        = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1];
    ctx.rawController[0][(uint8)WPadBtn::L5]        = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_LEFT_PADDLE2];
    ctx.rawController[0][(uint8)WPadBtn::R5]        = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2];

    if (ctx.controllerFeatures & (uint16)WEngine::ControllerFeatures::Touch_Click_Remap)
        ctx.rawController[0][(uint8)WPadBtn::Menu2] = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_TOUCHPAD];
    else
        ctx.rawController[0][(uint8)WPadBtn::Touchpad] = ctx.rawSDLController[0][SDL_GAMEPAD_BUTTON_TOUCHPAD];

    // Haptic doesnt read triggers yet.
    //ctx.rawController[0][(uint8)WPadBtn::LT] = ctx.rawSDLController[0][];
    //ctx.rawController[0][(uint8)WPadBtn::RT] = ctx.rawSDLController[0][];
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
        bool pressed = false;
        if (sense.keyboardCheck)
        {
            if (ctx.rawKeys[0][(uint8)sense.key])
                pressed = true;
        }
        if (sense.controllerCheck)
        {
            if (ctx.rawController[0][(uint8)sense.controller])
                pressed = true;
        }
        FetchResult fetch{};
        fetch.held = pressed;
        map.results[sense.name] = fetch;
    }
}

#endif
