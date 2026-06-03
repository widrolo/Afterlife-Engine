#pragma once

#include <Engine/EngineDefines.h>
#if GPU_BACKEND == GPU_VULKAN

struct VulkanStatistics
{
    uint32 drawCallsThisFrame = 0;
    uint32 drawCallsLastFrame = 0;
    uint64 vramUsage = 0;
};

#endif
