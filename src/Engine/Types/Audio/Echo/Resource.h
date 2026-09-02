#pragma once
#include <string>

#include "Engine/Types/CommonTypes.h"

namespace Echo
{
    enum class ChannelLayout : uint8
    {
        Mono,
        Stereo
    };

    struct SoundDesc
    {
        std::string name;
        const byte* audioBuffer; // this will be copied to an internal buffer. Also, only vorbis.
        sizeT sizeBytes = 0;
        uint32 sampleRate = 44100;
        ChannelLayout channelLayout = ChannelLayout::Stereo;
    };

    struct StreamDesc
    {
        std::string name;
        std::string filePath;
        ChannelLayout channelLayout = ChannelLayout::Stereo;
    };

    struct BusDesc
    {
        std::string name;
        float32 volume = 1.0f;
        float32 pitch = 1.0f;
    };
}
