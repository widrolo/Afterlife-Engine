#pragma once
#if HAPTIC_BACKEND == HAPTIC_SDL
#include "SDLContext.h"

void InitKeyboard(SDLContext &ctx);
void InitMouse(SDLContext &ctx);
void InitController(SDLContext &ctx);

void AdvanceBelts(SDLContext &ctx);

void CheckControllerStatus(SDLContext &ctx);
void CheckControllerStillConnected(SDLContext &ctx);
void CheckNewControllerConnected(SDLContext &ctx);

void UpdateControllerType(SDLContext &ctx);

SDLSense SenseToSDLSense(SDLContext &ctx, const WEngine::InputSense& sense);

#endif