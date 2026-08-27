#pragma once
#include "Engine/Types/CommonTypes.h"

enum class InvalidResultAction
{
    LetGo,
    Stall,
    Abort
};

struct GPUSettings
{
    _GLOBAL_CEX_ InvalidResultAction invalidHandleAction = InvalidResultAction::Abort;
};

struct GPUSettingsVulkan
{
    _GLOBAL_CEX_ bool useWAllocator = true;
#ifdef DEBUG
    _GLOBAL_CEX_ bool enableValidation = true;
#else
    _GLOBAL_CEX_ bool enableValidation = false;
#endif
    _GLOBAL_CEX_ InvalidResultAction invalidResultAction = InvalidResultAction::LetGo;
    _GLOBAL_CEX_ InvalidResultAction validationErrorAction = InvalidResultAction::Abort;
    _GLOBAL_CEX_ uint64 renderTargetsInFlightImages = 2;
};
