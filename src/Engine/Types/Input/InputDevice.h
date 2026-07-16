#pragma once

// This file does not contain LLM generated documentation

#include <string>
#include <SDL3/SDL.h>
#include <Engine/Math/Vector.h>
#include <Engine/Types/Rendering/Color.h>

#include "Engine/Util/BitwiseMacros.h"

namespace WEngine
{
    /**
     * Archetypes of input devices
     */
    enum class InputKind
    {
        Keyboard,
        Mouse,
        Controller,
    };

    /**
     * Used for knowing which icons to use in the UI and which features you could use in gameplay,
     */
    enum class InputVendor //nope, could not find a better name
    {
        KeyboardMouse,
        Xbox,
        PlayStation,
        Nintendo,
        Valve,
    };

    /**
     * Types of controllers supported by the engine.
     */
    enum class ControllerType
    {
        /**
         * Expected to include a D-Pad, Face buttons, 2 clickable dual-axis joysticks, 2 menu buttons,
         * Shoulder buttons, analog triggers
         */
        Generic,
        /**
         * Expected to include rumble, impulse triggers
         */
        Xbox,
        /**
         * Expected to include rumble, gyro, accelerometer, single addressable RGB LED
         * Note: PlayStation hardware expects the touchpad press to be a de facto menu 2 press. This
         * must be emulated on PC if a PC driver mapped it differently
         */
        PlayStation,
        /**
         * Expected to include rumble, gyro, accelerometer, visually swapped A/B and X/Y,
         * digital instead of analog triggers.
         */
        Nintendo,
        /**
         * Expected to include rumble, gyro, accelerometer, 2 clickable touchpads, 4 back buttons, touch
         * sensitive joysticks, touch sensitive grip
         * Note that this is exclusively speaking about the steam controller 2026, not the 2015 variant.
         */
        Valve,
    };

    /**
     * This is for bit masking features. Assuming all controllers support the above-mentioned Generic set.
     * Haptic has responsibility of dealing with this mess.
     * Don't add the distinction between the one PlayStation touchpad and the two Valve touchpads; Haptic
     * splits the PlayStation touchpad into two regions to emulate Valve.
     */
    enum class ControllerFeatures : word
    {
        Rumble                      = BIT(0),
        Impulse_Triggers            = BIT(1),
        Gyro                        = BIT(2),
        Accelerometer               = BIT(3),
        RGB_LED                     = BIT(4),
        Touch_Click_Remap           = BIT(5),
        ABXY_Remap                  = BIT(6),
        Digital_Triggers            = BIT(7),
        Back_Buttons                = BIT(8),
        Touch_Sensitive_Joysticks   = BIT(9),
        Touch_Sensitive_Grip        = BIT(10),
        Touchpad_Present            = BIT(11),
        Touchpad_Split              = BIT(12),
    };
}
