#pragma once

#include <Engine/EngineDefines.h>
#if GPU_BACKEND == GPU_VULKAN

#include "VulkanContext.h"

VkPipelineLayout CreatePipelineLayout(VulkanContext& ctx, VkDescriptorSetLayout descLayout);
VkPipelineLayout CreatePostProcessingPipelineLayout(VulkanContext& ctx, VkDescriptorSetLayout descLayout);

VkPipelineInputAssemblyStateCreateInfo CreatePipeline_InputAssembly();
VkPipelineInputAssemblyStateCreateInfo CreatePipeline_PostProcessInputAssembly();
VkPipelineVertexInputStateCreateInfo CreatePipeline_VertexDefinition();
VkPipelineVertexInputStateCreateInfo CreatePipeline_PostProcessVertexDefinition();
VkPipelineShaderStageCreateInfo CreatePipeline_ShaderStange_Vertex(const VulkanContext& ctx, const std::string &shaderName);
VkPipelineShaderStageCreateInfo CreatePipeline_ShaderStange_Fragment(const VulkanContext& ctx, const std::string &shaderName);
VkPipelineColorBlendAttachmentState CreatePipeline_ColorBlend(bool enableBlending);

#endif
