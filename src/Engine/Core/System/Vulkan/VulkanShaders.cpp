#include "VulkanShaders.h"

#include <Engine/EngineDefines.h>
#if GPU_BACKEND == GPU_VULKAN

#include <filesystem>

#include "VulkanHelpers.h"
#include "VulkanDescriptors.h"
#include "VulkanPipeline.h"
#include "Engine/Core/Engine.h"
#include "Engine/Core/Handlers/AssetRepo.h"
#include "Engine/Core/System/Iris.h"
#include "Engine/Core/System/OS.h"
#include "Engine/Types/AssetMission.h"
#include "Engine/Types/CoreSystems.h"
#include "Engine/Types/Nullable.h"
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

VkPipeline CreatePipeline(VulkanContext& ctx, const WEngine::ShaderDefinition& shaderDef, VkPipelineLayout pipelineLayout)
{
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
    depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilInfo.stencilTestEnable = VK_FALSE;
    if (shaderDef.depthTest)
    {
        depthStencilInfo.depthTestEnable = VK_TRUE;
        depthStencilInfo.depthWriteEnable = VK_TRUE;
    }

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

    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};
    shaderStages[0] = CreatePipeline_ShaderStange_Vertex(ctx, shaderDef.vertexCodeName);
    shaderStages[1] = CreatePipeline_ShaderStange_Fragment(ctx, shaderDef.fragmentCodeName);

    auto inputAssembly = CreatePipeline_InputAssembly();
    auto vertexDefinition = CreatePipeline_VertexDefinition();

    // This is basically attempting to make the compiler only call it once, we need it in pointer form anyway.
    // No clue if the compiler actually ends up only calling it once thought.
    static VkFormat swapFormat = FindBestSwapchainFormat(ctx);

    VkPipelineRenderingCreateInfo dynaRenderInfo{};
    dynaRenderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    dynaRenderInfo.colorAttachmentCount = 1;
    dynaRenderInfo.pColorAttachmentFormats = &swapFormat;
    dynaRenderInfo.depthAttachmentFormat = FindBestDepthFormat(ctx); // hypocrisy

    VkGraphicsPipelineCreateInfo pipeInfo{};
    pipeInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeInfo.pNext = &dynaRenderInfo;
    pipeInfo.pInputAssemblyState = &inputAssembly;
    pipeInfo.pVertexInputState = &vertexDefinition;
    pipeInfo.pRasterizationState = &rasterInfo;
    pipeInfo.pColorBlendState = &blendInfo;
    pipeInfo.pViewportState = &viewInfo;
    pipeInfo.pDepthStencilState = &depthStencilInfo;
    pipeInfo.pMultisampleState = &multisampleInfo;
    pipeInfo.pDynamicState = &dynamicInfo;
    pipeInfo.stageCount = shaderStages.size();
    pipeInfo.pStages = shaderStages.data();
    pipeInfo.layout = pipelineLayout;

    VkPipeline pipeline;
    auto res = vkCreateGraphicsPipelines(ctx.vcore.gpuDevice, VK_NULL_HANDLE, 1, &pipeInfo, ctx.vcore.allocator, &pipeline);

    if (!ParseVkResult(res))
    {
        WEngine::WLog::SetConsoleError();
        WEngine::WLog::ConsoleLog("Failed to create graphics pipeline.");
        return VK_NULL_HANDLE;
    }

    vkDestroyShaderModule(ctx.vcore.gpuDevice, shaderStages[0].module, ctx.vcore.allocator);
    vkDestroyShaderModule(ctx.vcore.gpuDevice, shaderStages[1].module, ctx.vcore.allocator);

    wFree((void*)vertexDefinition.pVertexBindingDescriptions);
    wFree((void*)vertexDefinition.pVertexAttributeDescriptions);

    return pipeline;
}

VkPipeline CreatePostProcessingPipeline(VulkanContext &ctx, const WEngine::ShaderDefinition &shaderDef,
    VkPipelineLayout pipelineLayout)
{
    VkPipelineRasterizationStateCreateInfo rasterInfo{};
    rasterInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterInfo.lineWidth = 1.0f;

    VkPipelineColorBlendAttachmentState blendState{};
    blendState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                VK_COLOR_COMPONENT_B_BIT;

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
    depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;

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

    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};
    shaderStages[0] = CreatePipeline_ShaderStange_Vertex(ctx, shaderDef.vertexCodeName);
    shaderStages[1] = CreatePipeline_ShaderStange_Fragment(ctx, shaderDef.fragmentCodeName);

    auto inputAssembly = CreatePipeline_PostProcessInputAssembly();
    auto vertexDefinition = CreatePipeline_PostProcessVertexDefinition();

    // This is basically attempting to make the compiler only call it once, we need it in pointer form anyway.
    // No clue if the compiler actually ends up only calling it once thought.
    static VkFormat swapFormat = FindBestSwapchainFormat(ctx);

    VkPipelineRenderingCreateInfo dynaRenderInfo{};
    dynaRenderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    dynaRenderInfo.colorAttachmentCount = 1;
    dynaRenderInfo.pColorAttachmentFormats = &swapFormat;
    dynaRenderInfo.depthAttachmentFormat = FindBestDepthFormat(ctx); // hypocrisy

    VkGraphicsPipelineCreateInfo pipeInfo{};
    pipeInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeInfo.pNext = &dynaRenderInfo;
    pipeInfo.pInputAssemblyState = &inputAssembly;
    pipeInfo.pVertexInputState = &vertexDefinition;
    pipeInfo.pRasterizationState = &rasterInfo;
    pipeInfo.pColorBlendState = &blendInfo;
    pipeInfo.pViewportState = &viewInfo;
    pipeInfo.pDepthStencilState = &depthStencilInfo;
    pipeInfo.pMultisampleState = &multisampleInfo;
    pipeInfo.pDynamicState = &dynamicInfo;
    pipeInfo.stageCount = shaderStages.size();
    pipeInfo.pStages = shaderStages.data();
    pipeInfo.layout = pipelineLayout;

    VkPipeline pipeline;

    auto res = vkCreateGraphicsPipelines(ctx.vcore.gpuDevice, VK_NULL_HANDLE, 1, &pipeInfo, ctx.vcore.allocator, &pipeline);

    if (!ParseVkResult(res))
    {
        WEngine::WLog::SetConsoleError();
        WEngine::WLog::ConsoleLog("Failed to create graphics pipeline.");
        return VK_NULL_HANDLE;
    }

    vkDestroyShaderModule(ctx.vcore.gpuDevice, shaderStages[0].module, ctx.vcore.allocator);
    vkDestroyShaderModule(ctx.vcore.gpuDevice, shaderStages[1].module, ctx.vcore.allocator);

    wFree((void*)vertexDefinition.pVertexBindingDescriptions);
    wFree((void*)vertexDefinition.pVertexAttributeDescriptions);

    return pipeline;
}

void SaturateDescriptorSet(VulkanContext& ctx, Vulkan_Material& material)
{
    auto& set = material.materialDescriptorSet;

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageView   = material.albedo;
    imageInfo.sampler     = material.albedoSampler;
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = set;
    write.dstBinding = 0;
    write.dstArrayElement = 0;
    write.descriptorCount = 1;
    write.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(ctx.vcore.gpuDevice, 1, &write, 0, nullptr);
}

void TryCompileAllShaders(VulkanContext &ctx)
{
    TryCompileAllMaterialShaders(ctx);
    TryCompileAllPostProcessingShaders(ctx);
}

void TryCompileAllMaterialShaders(VulkanContext &ctx)
{
    std::string dir = WEngine::CoreSystems::GetAssetRepo()->GetDataPath() + EngineSettings::shaderPath + "definitions";
    auto files = OS::GetAllFileNamesInDir(dir);

    for (auto& file : files)
    {
        std::filesystem::path p(file);
        file = p.stem().string();
    }

    for (const auto& file : files)
    {
        if (file == "$Sample")
            continue;

        if (WEngine::Engine::GetCla().testMode)
        {
            if (file != "TexturedUnlit")
                continue;
        }

        WEngine::YamlAssetMission mission;
        mission.name = "../" + EngineSettings::shaderPath + "definitions/" + file;
        WEngine::CoreSystems::GetAssetRepo()->GetAsset(mission);

        WEngine::ShaderDefinition def{};
        def.Parse(mission.root);
        if (!def.valid)
            continue;

        Vulkan_Shader shader{};

        auto descPool = CreateDescriptorPool(ctx, def);
        auto descLayout = CreateDescriptorSetLayout(ctx, def);
        auto pipeLayout = CreatePipelineLayout(ctx, descLayout);
        auto pipeline = CreatePipeline(ctx, def, pipeLayout);

        shader.descriptorPool = descPool;
        shader.descriptorSetLayout = descLayout;
        shader.pipelineLayout = pipeLayout;
        shader.pipeline = pipeline;
        shader.shaderDefinition = def;

        ctx.loadedShaders.push_back(shader);
        WEngine::Shader shaderHandle = ctx.loadedShaders.size();
        ctx.loadedShadersHandles[def.name] = shaderHandle;

        WEngine::WLog::SetConsoleSuccess();
        WEngine::WLog::ConsoleLog(std::format("Shader \"{}\" successfully compiled!", def.name));
    }
}

void TryCompileAllPostProcessingShaders(VulkanContext &ctx)
{
    if (WEngine::Engine::GetCla().testMode)
        return;
    std::string dir = WEngine::CoreSystems::GetAssetRepo()->GetDataPath() + EngineSettings::shaderPath + "definitions/PostProcessing";
    auto files = OS::GetAllFileNamesInDir(dir);

    for (auto& file : files)
    {
        std::filesystem::path p(file);
        file = p.stem().string();
    }

    for (const auto& file : files)
    {
        if (file == "$Sample")
            continue;

        WEngine::YamlAssetMission mission;
        mission.name = "../" + EngineSettings::shaderPath + "definitions/PostProcessing/" + file;
        WEngine::CoreSystems::GetAssetRepo()->GetAsset(mission);

        WEngine::ShaderDefinition def{};
        def.Parse(mission.root);
        if (!def.valid)
            continue;

        Vulkan_Shader shader{};

        auto pipeline = CreatePostProcessingPipeline(ctx, def, ctx.postProcessing.pipelineLayout);

        shader.pipeline = pipeline;
        shader.shaderDefinition = def;

        ctx.loadedShaders.push_back(shader);
        WEngine::Shader shaderHandle = ctx.loadedShaders.size();
        ctx.loadedShadersHandles[def.name] = shaderHandle;

        WEngine::WLog::SetConsoleSuccess();
        WEngine::WLog::ConsoleLog(std::format("Post Processing shader \"{}\" successfully compiled!", def.name));
    }
}

WEngine::Material CompileMaterial(VulkanContext &ctx, const std::string &matName)
{
    WEngine::YamlAssetMission mission;
    mission.name = "../Materials/" + matName;
    WEngine::CoreSystems::GetAssetRepo()->GetAsset(mission);

    WEngine::MaterialDefinition matDef;
    matDef.Parse(mission.root);

    if (!matDef.valid)
        return 0;

    // lets hope iris never calls this function through GetShader
    auto shaderN = Iris::GetShader(matDef.shaderName);

    if (!shaderN.HasValue())
        return 0;

    WEngine::IrisAssetCommunication comms{};
    comms.commType = WEngine::IrisAssetCommunicationType::GetMaterial;
    comms.matDef = matDef;

    WEngine::CoreSystems::GetAssetRepo()->IrisAssetComms(comms);


    Vulkan_Shader& shader = ctx.loadedShaders[shaderN.GetValue() - 1];
    Vulkan_Material mat;
    mat.materialShaderHandle = shaderN.GetValue();

    if (!matDef.texturesPackaging.empty())
    {
        mat.materialDescriptorSet = CreateDescriptorSet(ctx, shader);

        wtl::vector<VkWriteDescriptorSet> writeSets{};
        wtl::vector<VkDescriptorImageInfo> imageInfos(matDef.texturesPackaging.size());

        for (sizeT i = 0; i < matDef.texturesPackaging.size(); ++i)
        {
            Vulkan_Texture tex = ctx.loadedTextures[comms.texReferences[i] - 1];
            imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfos[i].imageView = tex.imageView;
            imageInfos[i].sampler = tex.sampler;
            imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;

            VkWriteDescriptorSet writeSet{};
            writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeSet.dstSet = mat.materialDescriptorSet;
            writeSet.dstBinding = i;
            writeSet.descriptorCount = 1;
            writeSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            writeSet.pImageInfo = &imageInfos[i];


            writeSets.push_back(writeSet);
        }

        vkUpdateDescriptorSets(ctx.vcore.gpuDevice, writeSets.size(), writeSets.data(), 0, nullptr);

        mat.hasTextures = true;
    }

    ctx.loadedMaterials.push_back(mat);
    WEngine::Material matHandle = ctx.loadedMaterials.size();
    ctx.loadedMaterialHandles[matDef.name] = matHandle;

    WEngine::WLog::SetConsoleSuccess();
    WEngine::WLog::ConsoleLog(std::format("Material \"{}\" compiled", matName));

    return matHandle;
}

void UpdateLighting(VulkanContext &ctx)
{
    RawLighting raw{};
    raw.sunDir = ctx.lighting.lightingInfo.sun.direction;
    raw.ambientIntensity = ctx.lighting.lightingInfo.ambient.intensity;
    raw.timeOfDay = ctx.lighting.lightingInfo.timeOfDay;

    WEngine::Color sunCol = ctx.lighting.lightingInfo.sun.lightColor;
    WEngine::Color ambCol = ctx.lighting.lightingInfo.ambient.ambientColor;
    raw.sunColor = {sunCol.red / 255.0f, sunCol.green / 255.0f, sunCol.blue / 255.0f};
    raw.ambientColor = {ambCol.red / 255.0f, ambCol.green / 255.0f, ambCol.blue / 255.0f};

    raw.camPos = ctx.lighting.lightingInfo.cameraPos;

    memcpy(ctx.lighting.lightAllocInfo.pMappedData, &raw, sizeof(RawLighting));
}

#endif
