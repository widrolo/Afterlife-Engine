#pragma once

#include <Engine/EngineDefines.h>
#if GPU_BACKEND == GPU_VULKAN

#include "VulkanContext.h"
#include "Engine/Types/Rendering/ShaderDefinition.h"

VkPipelineLayout CreatePipelineLayout(VulkanContext& ctx, VkDescriptorSetLayout descLayout);
VkPipelineLayout CreatePostProcessingPipelineLayout(VulkanContext& ctx, VkDescriptorSetLayout descLayout);
VkDescriptorSetLayout CreateDescriptorSetLayout(VulkanContext& ctx, const WEngine::ShaderDefinition& shaderDef);
VkDescriptorSetLayout CreatePostProcessingDescriptorSetLayout(VulkanContext& ctx);
VkDescriptorSet CreateDescriptorSet(VulkanContext& ctx, const Vulkan_Shader& shader);
bool SetupImGuiDescriptorPool(VulkanContext& ctx);
VkDescriptorPool CreateDescriptorPool(VulkanContext& ctx, const WEngine::ShaderDefinition& shaderDef);
VkDescriptorPool CreatePostProcessDescriptorPool(VulkanContext& ctx);
bool SetupLightingDescriptors(VulkanContext& ctx);

#endif