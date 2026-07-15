#if HAPTIC_BACKEND == HAPTIC_SDL

#include "SDLHelpers.h"
#include <algorithm>
#include "Engine/Util/Log.h"

WKey StringToWKey(const std::string &name)
{
    std::string nameLow = name;
    std::transform(nameLow.begin(), nameLow.end(), nameLow.begin(), ::tolower);
    for (uint64 i = 0; i < (uint8)WKey::WKey_Count; i++)
    {
        if (nameLow == WKey_Names[i])
            return (WKey)i;
    }
    WEngine::WLog::SetConsoleError();
    WEngine::WLog::ConsoleLog(std::format("Unable to parse key {}", name));
    return WKey::DEBUG12;
}

WMouseBtn StringToWMouseBtn(const std::string &name)
{

}

WPadBtn StringToWPadBtn(const std::string &name)
{
    std::string nameLow = name;
    std::transform(nameLow.begin(), nameLow.end(), nameLow.begin(), ::tolower);
    for (uint64 i = 0; i < (uint8)WPadBtn::WPadBtn_Count; i++)
    {
        if (nameLow == WPadBtn_Names[i])
            return (WPadBtn)i;
    }
    WEngine::WLog::SetConsoleError();
    WEngine::WLog::ConsoleLog(std::format("Unable to parse controller button {}", name));
    return WPadBtn::R5;
}

#endif
