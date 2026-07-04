#pragma once

#include <Engine/EngineDefines.h>
#if GPU_BACKEND == GPU_VULKAN

#include "VulkanContext.h"

bool AddModelToBLAS(VulkanContext& ctx, Vulkan_Model& model);

#endif
