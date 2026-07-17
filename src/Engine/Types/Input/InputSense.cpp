#include "InputSense.h"

using namespace WEngine;

std::string WEngine::FloatInputSensePad_Name[(sizeT)FloatInputSensePad::FloatInputSensePad_Count] =
{
    "ltrigger",
    "rtrigger",
    "ltriggerdelta",
    "rtriggerdelta",
    "ltouchpressure",
    "rtouchpressure",
    "ltouchpressuredelta",
    "rtouchpressuredelta"
};

std::string WEngine::FloatInputSenseMouse_Name[(sizeT)FloatInputSenseMouse::FloatInputSenseMouse_Count] =
{
    "speedchange",
    "wheelspeed"
};

std::string WEngine::VectorInputSensePad_Name[(sizeT)VectorInputSensePad::VectorInputSensePad_Count] =
{
    "ljoy",
    "rjoy",
    "ljoydelta",
    "rjoydelta",
    "ltouch",
    "rtouch",
    "ltouchdelta",
    "rtouchdelta"
};

std::string WEngine::VectorInputSenseMouse_Name[(sizeT)VectorInputSenseMouse::VectorInputSenseMouse_Count] =
{
    "mousedelta",
    "windowpos",
    "windowpossquare"
};