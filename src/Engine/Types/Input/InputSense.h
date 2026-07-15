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
