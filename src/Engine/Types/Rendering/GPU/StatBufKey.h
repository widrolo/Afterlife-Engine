#pragma once

#include <Engine/Types/CommonTypes.h>

namespace WEngine
{
    /**
      * Handle for a stationary buffer.
      */
    struct StatBufKey
    {
        StatBufKey() : handle(0) {};
        StatBufKey(uint64 handle) : handle(handle) {};
        operator uint64() const { return handle; }
        uint64 handle;
    };
}