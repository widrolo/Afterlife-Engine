#pragma once

#include <Engine/EngineDefines.h>
#if GPU_BACKEND == GPU_VULKAN

#include <vulkan/vulkan.h>
#include "VulkanContext.h"
#include "Engine/Types/AssetMission.h"

VkShaderModule CompileShader(const VulkanContext& ctx, const WEngine::SpirVAssetMission& spirvAssetMission);
VkPipeline CreatePipeline(VulkanContext& ctx, VkRenderPass renderPass, std::string shaderName);

#endif
