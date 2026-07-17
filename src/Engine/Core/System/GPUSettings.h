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
    _GLOBAL_CEX_ uint64 stationaryInstBufferSize = 64 * KB;
    _GLOBAL_CEX_ uint64 maxStationaryInstBuffers = 512;

    _GLOBAL_CEX_ InvalidResultAction invalidHandleAction = InvalidResultAction::Abort;
};

struct GPUSettingsOpenGL
{

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
    _GLOBAL_CEX_ uint64 maxInstanceBufferSize = 4;
    _GLOBAL_CEX_ uint64 maxMaterialCount = 512;
    _GLOBAL_CEX_ uint64 maxPPShaders = 512;
};
