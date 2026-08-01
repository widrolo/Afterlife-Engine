#pragma once

#define DEFINE_OPAQUE_HANDLE(T) struct T \
    { \
        T() : handle(0) {}; \
        T(uint64 handle) : handle(handle) {}; \
        operator uint64() const { return handle; } \
        uint64 handle; \
    };
