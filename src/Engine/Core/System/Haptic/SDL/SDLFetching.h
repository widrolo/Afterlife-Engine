#pragma once
#if HAPTIC_BACKEND == HAPTIC_SDL
#include "SDLContext.h"

void FetchAllInput(SDLContext &ctx);
void FetchKeyboard(SDLContext &ctx);
void FetchMouse(SDLContext &ctx);
void FetchController(SDLContext &ctx);

void TranslateFetched(SDLContext& ctx);
void TranslateKeyboard(SDLContext& ctx);
void TranslateMouse(SDLContext& ctx);
void TranslateController(SDLContext& ctx);

void UpdateAllSenses(SDLContext& ctx);
void UpdateAllSensesInMap(SDLContext& ctx, SDLMap& map);
FetchResult UpdateActionSense(const SDLContext &ctx, const SDLSense& sense);
FetchResult UpdateFloatSense(const SDLContext &ctx, const SDLSense& sense);
FetchResult UpdateVectorSense(const SDLContext &ctx, const SDLSense& sense);

#endif