#pragma once
#if GPU_BACKEND == GPU_VULKAN
#include "Engine/Core/System/Iris/Vulkan/IrisGlobals.h"

void PopulateSemsAndFencesSwapchain(Vulkan_RenderTargetSwapchain &rt);
void PopulateSemsAndFences(Vulkan_RenderTarget &rt);

#endif
