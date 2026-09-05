#pragma once

#include <miniaudio.h>
#include "Engine/Types/CommonTypes.h"

void EchoMiniAudioCallback(ma_device* dev, void* output, const void* input, uint32 frame_count);
