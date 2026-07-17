#if HAPTIC_BACKEND == HAPTIC_SDL

#include "SDLHelpers.h"
#include <algorithm>
#include "Engine/Util/Log.h"

WKey StringToWKey(const std::string &name)
{
    std::string nameLow = name;
    std::transform(nameLow.begin(), nameLow.end(), nameLow.begin(), ::tolower);
    for (sizeT i = 0; i < (uint8)WKey::WKey_Count; i++)
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
    for (sizeT i = 0; i < (uint8)WPadBtn::WPadBtn_Count; i++)
    {
        if (nameLow == WPadBtn_Names[i])
            return (WPadBtn)i;
    }
    WEngine::WLog::SetConsoleError();
    WEngine::WLog::ConsoleLog(std::format("Unable to parse controller button {}", name));
    return WPadBtn::R5;
}

WEngine::FloatInputSensePad StringToFloatInput(const std::string &name)
{
    std::string nameLow = name;
    std::transform(nameLow.begin(), nameLow.end(), nameLow.begin(), ::tolower);
    for (sizeT i = 0; i < (uint8)WEngine::FloatInputSensePad::FloatInputSensePad_Count; i++)
    {
        if (nameLow == WEngine::FloatInputSensePad_Name[i])
            return (WEngine::FloatInputSensePad)i;
    }
    WEngine::WLog::SetConsoleError();
    WEngine::WLog::ConsoleLog(std::format("Unable to parse controller float {}", name));
    return WEngine::FloatInputSensePad::lTouchPressureDelta;
}

WEngine::VectorInputSensePad StringToVectorInput(const std::string &name)
{
    std::string nameLow = name;
    std::transform(nameLow.begin(), nameLow.end(), nameLow.begin(), ::tolower);
    for (sizeT i = 0; i < (uint8) WEngine::VectorInputSensePad::VectorInputSensePad_Count; i++)
    {
        if (nameLow == WEngine::VectorInputSensePad_Name[i])
            return (WEngine::VectorInputSensePad)i;
    }
    WEngine::WLog::SetConsoleError();
    WEngine::WLog::ConsoleLog(std::format("Unable to parse controller vector {}", name));
    return WEngine::VectorInputSensePad::rTouchDelta;
}

bool ValidateActionRequest(const SDLContext& ctx, const std::string &name)
{
    if (ctx.selectedMap == nullptr)
        return false;
}

#endif
