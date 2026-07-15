#pragma once
#if HAPTIC_BACKEND == HAPTIC_SDL
#include <string>
#include "Engine/Types/Input/KeyCodes.h"

WKey StringToWKey(const std::string& name);
WMouseBtn StringToWMouseBtn(const std::string& name);
WPadBtn StringToWPadBtn(const std::string& name);

#endif