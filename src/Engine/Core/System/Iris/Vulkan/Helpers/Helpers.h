#pragma once
#if GPU_BACKEND == GPU_VULKAN
#include <string>
#include "Engine/Types/CommonTypes.h"
#include "Engine/Types/Rendering/DDS.h"
#include "Engine/WTL/vector.h"
#include <vulkan/vulkan.h>
#include "Engine/Math/Vector.h"
#include "Engine/Types/Rendering/Iris/FixedFunction.h"
#include "Engine/Types/Rendering/Iris/Formats.h"
#include "Engine/Types/Rendering/Iris/Resource.h"
#include "Engine/Types/Rendering/Iris/Usage.h"

// Since the swapchain is special, it gets to use the zero handle.
constexpr uint64 swapchainHandle = 0;

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

VkBufferUsageFlags IrisBuffUsageToVulkan(Iris::BufferUsage usage);
VkFormat IrisImgFormatToVulkan(Iris::ImgFormat format);
VkFilter IrisFilterToVulkan(Iris::FilterMode filter);
VkSamplerMipmapMode IrisMipFilterToVulkan(Iris::MipFilterMode mipFilter);
VkSamplerAddressMode IrisSamAddrModeToVulkan(Iris::SamplerAddressMode mode);
VkCompareOp IrisCompareOpToVulkan(Iris::CompareOp op);
VkCullModeFlags IrisCullModeToVulkan(Iris::CullMode mode);
VkFrontFace IrisFrontFaceToVulkan(Iris::FrontFace frontFace);
VkPolygonMode IrisFillModeToVulkan(Iris::FillMode fillMode);
VkBlendFactor IrisBlendFactorToVulkan(Iris::BlendFactor factor);
VkBlendOp IrisBlendOpToVulkan(Iris::BlendOp op);
VkStencilOp IrisStencilOpToVulkan(Iris::StencilOp op);
VkLogicOp IrisLogicOpToVulkan(Iris::LogicOp op);
VkBorderColor IrisBorderColorToVulkan(Iris::BorderColor color);
VkAttachmentLoadOp IrisLoadOpToVulkan(Iris::LoadOp op);
VkAttachmentStoreOp IrisStoreOpToVulkan(Iris::StoreOp op);
VkShaderStageFlags IrisShaderStageToVulkan(Iris::ShaderStage stage);
VkSampleCountFlagBits IrisSampleCountToVulkan(Iris::SampleCount count);
VkImageUsageFlags IrisTextureUsageToVulkan(Iris::TextureUsage usage);
VkDescriptorType IrisResourceTableEntryTypeToVulkan(Iris::ResourceTableEntryType type);
VkPrimitiveTopology IrisTopologyToVulkan(Iris::TopologyType topology);
VkFormat IrisVertFormatToVulkan(Iris::VertFormat format);
sizeT IrisVertFormatSize(Iris::VertFormat format);


VkFormat FindBestSwapchainFormat();
VkFormat FindBestDepthFormat();

VkCommandBuffer GetCurrentCmdBuff(Iris::CommandBufferHandle cmd);

wtl::vector<std::string> GetExtensionsToLoad();

#endif
