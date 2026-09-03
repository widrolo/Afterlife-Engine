#if ECHO_BACKEND == ECHO_MINIAUDIO

#include <Engine/Core/System/Echo.h>
#include <miniaudio.h>

namespace Echo
{
    VirtualAudioHandle CreateAudio(AudioSoundHandle sound)
    {

    }

    VirtualAudioHandle CreateAudio(AudioStreamHandle stream)
    {

    }

    VirtualAudioHandle SwapAudio(VirtualAudioHandle audio, AudioSoundHandle sound)
    {

    }

    VirtualAudioHandle SwapAudio(VirtualAudioHandle audio, AudioStreamHandle stream)
    {

    }

    void ConnectAudio(VirtualAudioHandle audio, AudioBusHandle bus)
    {

    }

    void SetAudioLoop(VirtualAudioHandle audio, bool loop)
    {

    }

    void SetAudioVolume(VirtualAudioHandle audio, bool volume)
    {

    }

    void SetAudioPitch(VirtualAudioHandle audio, bool pitch)
    {

    }
}

#endif
