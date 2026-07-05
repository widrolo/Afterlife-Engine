#include <Engine/EngineDefines.h>
#if GPU_BACKEND == GPU_VULKAN

#include "VulkanPipeline.h"

#include "VulkanHelpers.h"
#include "VulkanShaders.h"
#include "Engine/Core/Handlers/AssetRepo.h"
#include "Engine/Types/AssetMission.h"
#include "Engine/Types/CoreSystems.h"
#include "Engine/Types/Rendering/InstanceData.h"
#include "Engine/Util/Log.h"

VkPipelineLayout CreatePipelineLayout(VulkanContext& ctx, VkDescriptorSetLayout descLayout)
{
    std::array<VkPushConstantRange, 1> pushConstants;

    // mvp / vp
    pushConstants[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstants[0].offset = 0;
    pushConstants[0].size = sizeof(WEngine::Mat4x4);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pushConstantRangeCount = pushConstants.size();
    pipelineLayoutInfo.pPushConstantRanges = pushConstants.data();

    wtl::vector<VkDescriptorSetLayout> descriptorSetLayouts = { ctx.lighting.descriptorSetLayout };

    if (descLayout != VK_NULL_HANDLE)
        descriptorSetLayouts.push_back(descLayout);

    pipelineLayoutInfo.setLayoutCount = descriptorSetLayouts.size();
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

    VkPipelineLayout layout{};

    auto resLayout = vkCreatePipelineLayout(ctx.vcore.gpuDevice, &pipelineLayoutInfo, ctx.vcore.allocator, &layout);

    if (!ParseVkResult(resLayout))
    {
        WEngine::WLog::SetConsoleError();
        WEngine::WLog::ConsoleLog("Unable to create pipeline layout!");
        return VK_NULL_HANDLE;
    }
    return layout;
}

VkPipelineLayout CreatePostProcessingPipelineLayout(VulkanContext &ctx, VkDescriptorSetLayout descLayout)
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descLayout;

    VkPipelineLayout layout{};

    auto resLayout = vkCreatePipelineLayout(ctx.vcore.gpuDevice, &pipelineLayoutInfo, ctx.vcore.allocator, &layout);

    if (!ParseVkResult(resLayout))
    {
        WEngine::WLog::SetConsoleError();
        WEngine::WLog::ConsoleLog("Unable to create pipeline layout!");
        return VK_NULL_HANDLE;
    }
    return layout;
}

VkPipelineInputAssemblyStateCreateInfo CreatePipeline_InputAssembly()
{
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
    inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    return inputAssemblyInfo;
}

VkPipelineInputAssemblyStateCreateInfo CreatePipeline_PostProcessInputAssembly()
{
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
    inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    return inputAssemblyInfo;
}

VkPipelineVertexInputStateCreateInfo CreatePipeline_VertexDefinition()
{
    // it remains like this for now, but consider making this changeable pretty please
    const uint8 bindings = 2;
    const uint8 attributes = 9;

    auto bindDesc = wNewArr(VkVertexInputBindingDescription, bindings);
    bindDesc[0].binding = 0;
    bindDesc[0].stride = sizeof(WEngine::VertexData);
    bindDesc[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    bindDesc[1].binding = 1;
    bindDesc[1].stride = sizeof(WEngine::InstanceData);
    bindDesc[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

    // [0] = Position (Vector3)   |   [1] = Color (Vector3)   | [2] = Normals (Vector 3)
    // [3] = Color UV (Vector2)   |   [4] = Shadow UV (Vector2)
    // [5-7] = Model (Mat4x4)     |
    auto attributeDescriptions = wNewArr(VkVertexInputAttributeDescription, attributes);
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
    attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT; // Khronos had a meth party while making this one
    attributeDescriptions[2].offset = offsetof(WEngine::VertexData, normal);
    attributeDescriptions[3].binding = 0;
    attributeDescriptions[3].location = 3;
    attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT; // Khronos had a meth party while making this one
    attributeDescriptions[3].offset = offsetof(WEngine::VertexData, uv0Coord);
    attributeDescriptions[4].binding = 0;
    attributeDescriptions[4].location = 4;
    attributeDescriptions[4].format = VK_FORMAT_R32G32_SFLOAT; // Khronos had a meth party while making this one
    attributeDescriptions[4].offset = offsetof(WEngine::VertexData, uv1Coord);

    attributeDescriptions[5] = {5, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(float) * 0};
    attributeDescriptions[6] = {6, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(float) * 4};
    attributeDescriptions[7] = {7, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(float) * 8};
    attributeDescriptions[8] = {8, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(float) * 12};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = bindings;
    vertexInputInfo.pVertexBindingDescriptions = bindDesc;
    vertexInputInfo.vertexAttributeDescriptionCount = attributes;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;

    return vertexInputInfo;
}

VkPipelineVertexInputStateCreateInfo CreatePipeline_PostProcessVertexDefinition()
{
    // it remains like this for now, but consider making this changeable pretty please
    const uint8 bindings = 1;
    const uint8 attributes = 2;

    auto bindDesc = wNewArr(VkVertexInputBindingDescription, bindings);
    bindDesc[0].binding = 0;
    bindDesc[0].stride = sizeof(WEngine::PPVertexData);
    bindDesc[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    // [0] = Position (Vector3) | [1] = UV (Vector2)
    auto attributeDescriptions = wNewArr(VkVertexInputAttributeDescription, attributes);
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT; // Khronos had a meth party while making this one
    attributeDescriptions[0].offset = offsetof(WEngine::PPVertexData, position);
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT; // Khronos had a meth party while making this one
    attributeDescriptions[1].offset = offsetof(WEngine::PPVertexData, uv);

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = bindings;
    vertexInputInfo.pVertexBindingDescriptions = bindDesc;
    vertexInputInfo.vertexAttributeDescriptionCount = attributes;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;

    return vertexInputInfo;
}

VkPipelineShaderStageCreateInfo CreatePipeline_ShaderStange_Vertex(const VulkanContext& ctx, const std::string& shaderName)
{
    // Sperm Vee
    WEngine::SpirVAssetMission vertexShaderCode{};
    vertexShaderCode.shaderType = WEngine::SpirVAssetMission::VertexShader;
    vertexShaderCode.name = shaderName;
    WEngine::CoreSystems::GetAssetRepo()->GetAsset(vertexShaderCode);

    VkPipelineShaderStageCreateInfo shaderStage{};

    shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStage.pName = "main";
    shaderStage.module = CompileShader(ctx, vertexShaderCode);
    wFree(vertexShaderCode.shaderCode);

    return shaderStage;
}

VkPipelineShaderStageCreateInfo CreatePipeline_ShaderStange_Fragment(const VulkanContext& ctx, const std::string& shaderName)
{
    WEngine::SpirVAssetMission fragmentShaderCode{};
    fragmentShaderCode.shaderType = WEngine::SpirVAssetMission::FragmentShader;
    fragmentShaderCode.name = shaderName;
    WEngine::CoreSystems::GetAssetRepo()->GetAsset(fragmentShaderCode);

    VkPipelineShaderStageCreateInfo shaderStage{};

    shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStage.pName = "main";
    shaderStage.module = CompileShader(ctx, fragmentShaderCode);
    wFree(fragmentShaderCode.shaderCode);

    return shaderStage;
}

#endif

