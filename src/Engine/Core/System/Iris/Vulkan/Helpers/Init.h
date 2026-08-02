#pragma once
#if GPU_BACKEND == GPU_VULKAN

bool SetupInstance();
bool SetupValidation();
bool SetupDevice();

#endif