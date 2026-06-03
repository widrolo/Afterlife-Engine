#include "VulkanShaders.h"

#include <Engine/EngineDefines.h>
#if GPU_BACKEND == GPU_VULKAN

#include "VulkanHelpers.h"
#include "Engine/Core/Handlers/AssetRepo.h"
#include "Engine/Types/AssetMission.h"
#include "Engine/Types/CoreSystems.h"
#include "Engine/Types/Rendering/InstanceData.h"
#include "Engine/Types/Rendering/VertextData.h"
#include "Engine/Util/Log.h"

VkShaderModule CompileShader(const VulkanContext& ctx, const WEngine::SpirVAssetMission& spirvAssetMission)
{
    VkShaderModule module;
    VkShaderModuleCreateInfo shaderModuleInfo{};
    shaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleInfo.codeSize = spirvAssetMission.shaderSize;
    shaderModuleInfo.pCode = spirvAssetMission.shaderCode;

    auto res = vkCreateShaderModule(ctx.vcore.gpuDevice, &shaderModuleInfo, ctx.vcore.allocator, &module);
    if (!ParseVkResult(res))
    {
        WEngine::WLog::SetConsoleWarning();
        WEngine::WLog::ConsoleLog("Unable to create shader module!");
        return VK_NULL_HANDLE;
    }
    return module;
}

VkPipeline CreatePipeline(VulkanContext& ctx, VkRenderPass renderPass, std::string shaderName)
{
    VkPipeline pipeline;

    // ----------------------------------------------- [INPUT ASSEMBLY] -----------------------------------------------

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
    inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    // --------------------------------------------- [VERTEX DEFINITION] ----------------------------------------------

    std::array<VkVertexInputBindingDescription, 2> bindDesc{};
    bindDesc[0].binding = 0;
    bindDesc[0].stride = sizeof(WEngine::VertexData);
    bindDesc[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    bindDesc[1].binding = 1;
    bindDesc[1].stride = sizeof(WEngine::InstanceData);
    bindDesc[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

    // [0] = Position (Vector3)   |   [1] = Color (Vector3)   |   [2] = Color UV (Vector2)   |   [3] = Shadow UV (Vector2)
    // [4-7] = Model (Mat4x4)     |
    std::array<VkVertexInputAttributeDescription, 8> attributeDescriptions{};
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT; // Khronos had a meth party while making this one
    attributeDescriptions[0].offset = offsetof(WEngine::VertexData, position);
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT; // Khronos had a meth party while making this one
    attributeDescriptions[1].offset = offsetof(WEngine::VertexData, vertColor);
    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT; // Khronos had a meth party while making this one
    attributeDescriptions[2].offset = offsetof(WEngine::VertexData, uv0Coord);
    attributeDescriptions[3].binding = 0;
    attributeDescriptions[3].location = 3;
    attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT; // Khronos had a meth party while making this one
    attributeDescriptions[3].offset = offsetof(WEngine::VertexData, uv1Coord);

    attributeDescriptions[4] = {4, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(float) * 0};
    attributeDescriptions[5] = {5, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(float) * 4};
    attributeDescriptions[6] = {6, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(float) * 8};
    attributeDescriptions[7] = {7, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(float) * 12};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = bindDesc.size();
    vertexInputInfo.pVertexBindingDescriptions = bindDesc.data();
    vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    // ------------------------------------------ [RASTERIZER AND RENDERING] ------------------------------------------

    VkPipelineRasterizationStateCreateInfo rasterInfo{};
    rasterInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterInfo.lineWidth = 1.0f;

    VkPipelineColorBlendAttachmentState blendState{};
    blendState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo blendInfo{};
    blendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blendInfo.attachmentCount = 1;
    blendInfo.pAttachments = &blendState;

    VkPipelineViewportStateCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewInfo.viewportCount = 1;
    viewInfo.scissorCount = 1;

    VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
    depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilInfo.depthTestEnable = VK_TRUE;
    depthStencilInfo.depthWriteEnable = VK_TRUE;
    depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilInfo.stencilTestEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampleInfo{};
    multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    std::array<VkDynamicState, 2> dynamics{};
    dynamics[0] = VK_DYNAMIC_STATE_VIEWPORT;
    dynamics[1] = VK_DYNAMIC_STATE_SCISSOR;

    VkPipelineDynamicStateCreateInfo dynamicInfo{};
    dynamicInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicInfo.dynamicStateCount = dynamics.size();
    dynamicInfo.pDynamicStates = dynamics.data();

    // -------------------------------------------------- [SHADERS] ---------------------------------------------------

    // Sperm Vee
    WEngine::SpirVAssetMission vertexShaderCode{};
    vertexShaderCode.shaderType = WEngine::SpirVAssetMission::VertexShader;
    vertexShaderCode.name = shaderName;
    WEngine::SpirVAssetMission fragmentShaderCode{};
    fragmentShaderCode.shaderType = WEngine::SpirVAssetMission::FragmentShader;
    fragmentShaderCode.name = shaderName;
    WEngine::CoreSystems::GetAssetRepo()->GetAsset(vertexShaderCode);
    WEngine::CoreSystems::GetAssetRepo()->GetAsset(fragmentShaderCode);

    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};

    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].pName = "main";
    shaderStages[0].module = CompileShader(ctx, vertexShaderCode);
    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].pName = "main";
    shaderStages[1].module = CompileShader(ctx, fragmentShaderCode);

    VkGraphicsPipelineCreateInfo pipeInfo{};
    pipeInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeInfo.pInputAssemblyState = &inputAssemblyInfo;
    pipeInfo.pVertexInputState = &vertexInputInfo;
    pipeInfo.pRasterizationState = &rasterInfo;
    pipeInfo.pColorBlendState = &blendInfo;
    pipeInfo.pViewportState = &viewInfo;
    pipeInfo.pDepthStencilState = &depthStencilInfo;
    pipeInfo.pMultisampleState = &multisampleInfo;
    pipeInfo.pDynamicState = &dynamicInfo;
    pipeInfo.stageCount = shaderStages.size();
    pipeInfo.pStages = shaderStages.data();
    pipeInfo.renderPass = renderPass;
    pipeInfo.layout = ctx.pipelineLayout;

    auto res = vkCreateGraphicsPipelines(ctx.vcore.gpuDevice, VK_NULL_HANDLE, 1, &pipeInfo, ctx.vcore.allocator, &pipeline);

    if (!ParseVkResult(res))
    {
        WEngine::WLog::SetConsoleError();
        WEngine::WLog::ConsoleLog("Failed to create graphics pipeline.");
        return VK_NULL_HANDLE;
    }

    vkDestroyShaderModule(ctx.vcore.gpuDevice, shaderStages[0].module, ctx.vcore.allocator);
    vkDestroyShaderModule(ctx.vcore.gpuDevice, shaderStages[1].module, ctx.vcore.allocator);

    wFree(vertexShaderCode.shaderCode);
    wFree(fragmentShaderCode.shaderCode);

    return pipeline;
}

#endif