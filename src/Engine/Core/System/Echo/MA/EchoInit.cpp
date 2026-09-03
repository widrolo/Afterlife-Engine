#if ECHO_BACKEND == ECHO_MINIAUDIO

#include <Engine/Core/System/Echo.h>
#include <miniaudio.h>

namespace Echo
{
    bool Init(const InitDesc& desc)
    {
        ma_device_config config;
        config.playback.format = ma_format_unknown;
    }
    void Shutdown()
    {

    }
}

#endif
