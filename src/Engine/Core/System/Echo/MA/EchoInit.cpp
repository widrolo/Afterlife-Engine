#if ECHO_BACKEND == ECHO_MINIAUDIO

#include <Engine/Core/System/Echo.h>
#include "Helpers/Callback.h"
#include "EchoGlobals.h"
#include <miniaudio.h>

#include "Engine/Util/Log.h"

namespace Echo
{
    bool Init(const InitDesc& desc)
    {
        ma_device_config config = ma_device_config_init(ma_device_type_playback);
        config.dataCallback = EchoMiniAudioCallback;
        config.pUserData = nullptr;

        if (ma_device_init(NULL, &config, &device) != MA_SUCCESS)
        {
            WEngine::WLog::SetConsoleError();
            WEngine::WLog::ConsoleLog("Mini Audio failed to initialize.");
            return false;
        }

        ma_device_start(&device);

        return true;
    }
    void Shutdown()
    {
        ma_device_uninit(&device);
    }
}

#endif
