#pragma once
#if GPU_BACKEND == GPU_VULKAN
#include <string>
#include "Engine/Types/CommonTypes.h"
#include "Engine/Types/Rendering/DDS.h"
#include "Engine/WTL/vector.h"
#include <vulkan/vulkan.h>
#include "Engine/Math/Vector.h"

void PrintNotImplemented(const std::string& functionName);
bool ParseVkResult(VkResult result);
VkBool32 ValidationCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*);
void PerformInvalidHandleAction();
uint64 CalcTextureSize(uint8 bytesPerPixel, uint32 width, uint32 height);
sizeT BCBlockSize(BC format);
sizeT BCMipSize(uint32 width, uint32 height, BC format);
uint64 CalcModelSize(uint8 bytesPerVertex, uint32 vertexCount);
uint64 GetSizeOfImageInBytes(WEngine::Vector2 imageSize, uint8 channelCount);
uint32 GetVulkanVersion();

VkFormat FindBestSwapchainFormat();

wtl::vector<std::string> GetExtensionsToLoad();

#endif
