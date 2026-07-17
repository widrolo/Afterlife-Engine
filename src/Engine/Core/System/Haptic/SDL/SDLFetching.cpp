#if HAPTIC_BACKEND == HAPTIC_SDL

#include "SDLFetching.h"

#include <cstring>

#include "Engine/Util/Log.h"

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

    ctx.rawLeftTrigger[0] = SDL_GetGamepadAxis(ctx.controller, SDL_GAMEPAD_AXIS_LEFT_TRIGGER);
    ctx.rawRightTrigger[0] = SDL_GetGamepadAxis(ctx.controller, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);

    WEngine::Vector2 lJoy;
    WEngine::Vector2 rJoy;
    lJoy.x = SDL_GetGamepadAxis(ctx.controller, SDL_GAMEPAD_AXIS_LEFTX);
    lJoy.y = -SDL_GetGamepadAxis(ctx.controller, SDL_GAMEPAD_AXIS_LEFTY);
    rJoy.x = SDL_GetGamepadAxis(ctx.controller, SDL_GAMEPAD_AXIS_RIGHTX);
    rJoy.y = -SDL_GetGamepadAxis(ctx.controller, SDL_GAMEPAD_AXIS_RIGHTY);
    ctx.rawLeftJoy[0] = lJoy;
    ctx.rawRightJoy[0] = rJoy;
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
        switch (sense.inputKind)
        {
            case WEngine::InputSenseKind::Action:
                map.results[sense.name] = UpdateActionSense(ctx, sense);
                break;
            case WEngine::InputSenseKind::Float:
                map.results[sense.name] = UpdateFloatSense(ctx, sense);
                break;
            case WEngine::InputSenseKind::Vector:
                map.results[sense.name] = UpdateVectorSense(ctx, sense);
                break;
        }

    }
}

FetchResult UpdateActionSense(const SDLContext &ctx, const SDLSense &sense)
{
    bool justPressed = false;
    bool pressed = false;
    bool justReleased = false;

    bool canPossiblyJP = true;
    bool wasDownLastFrame = false;
    if (sense.keyboardCheck)
    {
        if (ctx.rawKeys[0][(sizeT)sense.key[0]] && !ctx.rawKeys[1][(sizeT)sense.key[0]])
            justPressed = true;
        if (ctx.rawKeys[0][(sizeT)sense.key[0]])
            pressed = true;
        if (!ctx.rawKeys[0][(sizeT)sense.key[0]] && ctx.rawKeys[1][(sizeT)sense.key[0]])
            justReleased = true;

        if (ctx.rawKeys[0][(sizeT)sense.key[0]] && ctx.rawKeys[1][(sizeT)sense.key[0]])
            canPossiblyJP = false;
        if (ctx.rawKeys[1][(sizeT)sense.key[0]])
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
        if (ctx.rawKeys[1][(sizeT)sense.key[0]])
            wasDownLastFrame = true;
    }
    FetchResult fetch{};
    if (canPossiblyJP && !wasDownLastFrame)
        fetch.justPressed = justPressed;
    fetch.held = pressed;
    if (!pressed)
        fetch.justReleased = justReleased;
    return fetch;
}

FetchResult UpdateFloatSense(const SDLContext &ctx, const SDLSense &sense)
{
    FetchResult fetch{};
    float32 kbTrigger = 0;
    float32 conTrigger = 0;
    if (sense.keyboardCheck)
    {
        if (ctx.rawKeys[0][(sizeT)sense.key[0]])
            kbTrigger -= 1;
        if (ctx.rawKeys[0][(sizeT)sense.key[1]])
            kbTrigger += 1;
    }
    if (sense.controllerCheck)
    {
        switch (sense.trigger)
        {
            case WEngine::FloatInputSensePad::lTrigger:
                conTrigger = ctx.rawLeftTrigger[0] / (float32)max_int16;
                break;
            case WEngine::FloatInputSensePad::rTrigger:
                conTrigger = ctx.rawRightTrigger[0] / (float32)max_int16;
                break;
            case WEngine::FloatInputSensePad::lTriggerDelta:
                conTrigger = (ctx.rawLeftTrigger[1] - ctx.rawLeftTrigger[0]) / (float32)max_int16;
                break;
            case WEngine::FloatInputSensePad::rTriggerDelta:
                conTrigger = (ctx.rawRightTrigger[1] - ctx.rawRightTrigger[0]) / (float32)max_int16;
                break;
            case WEngine::FloatInputSensePad::lTouchPressure:
                break;
            case WEngine::FloatInputSensePad::rTouchPressure:
                break;
            case WEngine::FloatInputSensePad::lTouchPressureDelta:
                break;
            case WEngine::FloatInputSensePad::rTouchPressureDelta:
                break;
            default: ;
        }
    }

    // same as checking which one is currently in use.
    fetch.trigger = (std::abs(kbTrigger) > std::abs(conTrigger)) ? kbTrigger : conTrigger;

    return fetch;
}

FetchResult UpdateVectorSense(const SDLContext &ctx, const SDLSense &sense)
{
    FetchResult fetch{};
    WEngine::Vector2 kbVec = WEngine::Vector2::Zero;
    WEngine::Vector2 conVec = WEngine::Vector2::Zero;
    if (sense.keyboardCheck)
    {
        if (ctx.rawKeys[0][(sizeT)sense.key[0]])
            kbVec.x -= 1;
        if (ctx.rawKeys[0][(sizeT)sense.key[1]])
            kbVec.x += 1;
        if (ctx.rawKeys[0][(sizeT)sense.key[2]])
            kbVec.y += 1;
        if (ctx.rawKeys[0][(sizeT)sense.key[3]])
            kbVec.y -= 1;
    }
    if (sense.controllerCheck)
    {
        switch (sense.joy)
        {
            case WEngine::VectorInputSensePad::lJoy:
                conVec = ctx.rawLeftJoy[0] / (float32)max_int16;
                break;
            case WEngine::VectorInputSensePad::rJoy:
                conVec = ctx.rawRightJoy[0] / (float32)max_int16;
                break;
            case WEngine::VectorInputSensePad::lJoyDelta:
                conVec = (ctx.rawLeftJoy[1] - ctx.rawLeftJoy[0]) / (float32)max_int16;
                break;
            case WEngine::VectorInputSensePad::rJoyDelta:
                conVec = (ctx.rawRightJoy[1] - ctx.rawRightJoy[0]) / (float32)max_int16;
                break;
            case WEngine::VectorInputSensePad::lTouch:
                break;
            case WEngine::VectorInputSensePad::rTouch:
                break;
            case WEngine::VectorInputSensePad::lTouchDelta:
                break;
            case WEngine::VectorInputSensePad::rTouchDelta:
                break;
            case WEngine::VectorInputSensePad::VectorInputSensePad_Count:
                break;
        }
    }

    //kbVec.NormaliseThis();

    // const for now
    const float deadzone = 0.15f;

    if (WEngine::Vector2::Magnitude(conVec) < deadzone)
        conVec = WEngine::Vector2::Zero;
    //else
    //    conVec.NormaliseThis();

    fetch.pos = (WEngine::Vector2::SqrMagnitude(kbVec) > WEngine::Vector2::SqrMagnitude(conVec)) ? kbVec : conVec;

    return fetch;
}

#endif
