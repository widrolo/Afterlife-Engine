#pragma once
#if HAPTIC_BACKEND == HAPTIC_SDL
#include "SDLContext.h"

void FetchAllInput(SDLContext &ctx);
void FetchKeyboard(SDLContext &ctx);
void FetchMouse(SDLContext &ctx);
void FetchController(SDLContext &ctx);

void UpdateAllSenses(SDLContext& ctx);
void UpdateAllSensesInMap(SDLContext& ctx, SDLMap& map);

#endif