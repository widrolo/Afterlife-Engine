#pragma once
#if GPU_BACKEND == GPU_VULKAN

#include <Engine/Core/System/Iris.h>

using namespace Iris;

struct IrisContext
{

};

_GLOBAL_ IrisContext ctx;

#endif