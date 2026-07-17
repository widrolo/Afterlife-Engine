#pragma once
#if HAPTIC_BACKEND == HAPTIC_SDL
#include <string>

#include "SDLContext.h"
#include "Engine/Types/Input/KeyCodes.h"

WKey StringToWKey(const std::string& name);
WMouseBtn StringToWMouseBtn(const std::string& name);
WPadBtn StringToWPadBtn(const std::string& name);
WEngine::FloatInputSensePad StringToFloatInput(const std::string& name);
WEngine::VectorInputSensePad StringToVectorInput(const std::string& name);

bool ValidateActionRequest(const SDLContext& ctx, const std::string& name);

#endif