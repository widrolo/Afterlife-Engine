#pragma once

#include <array>
#include <string>
#include <variant>

#include "Engine/Types/CommonTypes.h"

namespace WEngine
{
    enum class InputSenseKind
    {
        Action,
        Float,
        Vector
    };

    enum class FloatInputSensePad
    {
        lTrigger,
        rTrigger,
        lTriggerDelta,
        rTriggerDelta,
        lTouchPressure,
        rTouchPressure,
        lTouchPressureDelta,
        rTouchPressureDelta,

        FloatInputSensePad_Count
    };

    extern std::string FloatInputSensePad_Name[];

    enum class FloatInputSenseMouse
    {
        SpeedChange,
        WheelSpeed,

        FloatInputSenseMouse_Count
    };

    extern std::string FloatInputSenseMouse_Name[];

    enum class VectorInputSensePad
    {
        lJoy,
        rJoy,
        lJoyDelta,
        rJoyDelta,
        lTouch,
        rTouch,
        lTouchDelta,
        rTouchDelta,

        VectorInputSensePad_Count
    };

    extern std::string VectorInputSensePad_Name[];

    enum class VectorInputSenseMouse
    {
        MouseDelta,
        WindowPos,
        WindowPosSquare,

        VectorInputSenseMouse_Count
    };

    extern std::string VectorInputSenseMouse_Name[];

    struct InputActionInternal
    {
        std::string keyName;
        std::string buttonName;
        std::string mouseName;
    };

    struct InputFloatInternal
    {
        std::array<std::string, 2> keyNames;
        std::string triggerName;
        std::string sensorName;
        float32 mouseMaxSpeed;
    };

    struct InputVectorInternal
    {
        std::array<std::string, 4> keyNames;
        std::string joyName;
        std::string sensorName;
    };

    struct InputSense
    {
        std::string senseName;
        std::variant<InputActionInternal, InputFloatInternal, InputVectorInternal> inputInternal;
        InputSenseKind Kind() const
        {
            return (InputSenseKind)inputInternal.index();
        }
    };
}
