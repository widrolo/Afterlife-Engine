#pragma once
#include <string>
#include "InputDevice.h"
#include "Engine/WTL/vector.h"

namespace WEngine
{

    struct InputPatch
    {
        std::string map;
        std::string input;
        InputKind kind;
        wtl::vector<std::string> newInput;
    };
}
