#if GPU_BACKEND == GPU_VULKAN

#include <unistd.h>
#include <vk_mem_alloc.h>
#include <Engine/Core/System/Iris.h>
#include <Engine/Util/Log.h>

#include "IrisGlobals.h"
#include "Helpers/Helpers.h"
#include "Helpers/Types.h"

// Just up front: new handles are made by storage.size() after push, therefore 0 is only ever invalid.

namespace Iris
{
    BufferHandle CreateBuffer(const BufferDesc& desc)
    {
        return CreateBuffer(desc, nullptr, 0);
    }

    BufferHandle CreateBuffer(const BufferDesc& desc, const byte* initialData, sizeT initialDataSize)
    {
        Vulkan_Buffer buff;
        buff.debugName = desc.debugName;
        VkBufferCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        info.size = desc.size;
        info.usage = IrisBuffUsageToVulkan(desc.usage);

        VmaAllocationCreateInfo allocInfo{};
        // Due to the fact that we never unload data, we can just do this and never unmap it.
        allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        auto ret = vmaCreateBuffer(vcore.vmaAllocator, &info, &allocInfo, &buff.buffer, &buff.alloc, &buff.allocInfo);

        if (!ParseVkResult(ret))
        {
            WEngine::WLog::SetConsoleError();
            WEngine::WLog::ConsoleLog(std::format("Unable to create buffer \"{}\"!", desc.debugName));
            return 0;
        }

        stats.vramUsage += buff.allocInfo.size;

        loadedBuffers.push_back(buff);
        const BufferHandle handle = loadedBuffers.size();

        if (initialData == nullptr || initialDataSize == 0)
            return handle;

        const sizeT size = std::min(initialDataSize, (sizeT)buff.allocInfo.size);

        if (size != initialDataSize)
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog(std::format("Unable to copy the entire initial data into buffer \"{}\"!",
                desc.debugName));
        }

        memcpy(buff.allocInfo.pMappedData, initialData, size);

        return handle;
    }

    TextureHandle CreateTexture(const TextureDesc& desc)
    {
        Vulkan_Texture tex{};
        tex.debugName = desc.debugName;
        VkImageCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.imageType = VK_IMAGE_TYPE_2D;
        info.extent = {
            std::max(4u, (desc.width + 3u) & ~3u),
            std::max(4u, (desc.height + 3u) & ~3u),
            1
        };
        info.mipLevels = desc.mipLevels;
        info.arrayLayers = desc.arrayLayers;
        info.format = IrisImgFormatToVulkan(desc.format);
        info.tiling = VK_IMAGE_TILING_OPTIMAL;
        info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        info.usage = IrisTextureUsageToVulkan(desc.usage);
        info.samples = IrisSampleCountToVulkan(desc.sampleCount);

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        auto res = vmaCreateImage(vcore.vmaAllocator, &info, &allocInfo, &tex.image, &tex.alloc, &tex.allocInfo);


        if (!ParseVkResult(res))
        {
            WEngine::WLog::SetConsoleError();
            WEngine::WLog::ConsoleLog(std::format("Unable to create image \"{}\".", tex.debugName));
            return 0;
        }

        stats.vramUsage += tex.allocInfo.size;

        VkImageViewCreateInfo imageViewInfo{};
        imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewInfo.image = tex.image;
        imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewInfo.format = IrisImgFormatToVulkan(desc.format);
        imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewInfo.subresourceRange.baseMipLevel = 0;
        imageViewInfo.subresourceRange.levelCount = desc.mipLevels;
        imageViewInfo.subresourceRange.baseArrayLayer = 0;
        imageViewInfo.subresourceRange.layerCount = desc.arrayLayers;

        res = vkCreateImageView(vcore.gpuDevice, &imageViewInfo, vcore.allocator, &tex.imageView);

        if (!ParseVkResult(res))
        {
            stats.vramUsage -= tex.allocInfo.size;
            vmaDestroyImage(vcore.vmaAllocator, tex.image, tex.alloc);
            WEngine::WLog::SetConsoleError();
            WEngine::WLog::ConsoleLog(std::format("Unable to create image view for texture \"{}\".", tex.debugName));
            return 0;
        }

        loadedTextures.push_back(tex);
        return loadedTextures.size();
    }

    SamplerHandle CreateSampler(const SamplerDesc& desc)
    {
        Vulkan_Sampler sam{};
        sam.debugName = desc.debugName;

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = IrisFilterToVulkan(desc.magFilter);
        samplerInfo.minFilter = IrisFilterToVulkan(desc.minFilter);
        samplerInfo.mipmapMode = IrisMipFilterToVulkan(desc.mipFilter);
        samplerInfo.addressModeU = IrisSamAddrModeToVulkan(desc.addressU);
        samplerInfo.addressModeV = IrisSamAddrModeToVulkan(desc.addressV);
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT; // engine explicitly doesnt support 3D textures anyway
        samplerInfo.mipLodBias = desc.mipLodBias;
        samplerInfo.anisotropyEnable = desc.anisotropyEnable;
        samplerInfo.maxAnisotropy = desc.maxAnisotropy;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = VK_LOD_CLAMP_NONE;
        samplerInfo.compareEnable = desc.compareEnable;
        samplerInfo.compareOp = IrisCompareOpToVulkan(desc.compareOp);
        samplerInfo.borderColor = IrisBorderColorToVulkan(desc.borderColor);

        auto res = vkCreateSampler(vcore.gpuDevice, &samplerInfo, vcore.allocator, &sam.sampler);

        if (!ParseVkResult(res))
        {
            WEngine::WLog::SetConsoleError();
            WEngine::WLog::ConsoleLog(std::format("Unable to create sampler\"{}\".", sam.debugName));
            return 0;
        }

        loadedSamplers.push_back(sam);
        return loadedSamplers.size();
    }

    ShaderHandle CreateShader(const ShaderStageDesc& desc)
    {
        Vulkan_Shader shader{};
        shader.debugName = desc.debugName;
        shader.stage = IrisShaderStageToVulkan(desc.stage);

        VkShaderModuleCreateInfo shaderModuleInfo{};
        shaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleInfo.codeSize = desc.bytecodeSize;
        shaderModuleInfo.pCode = (const dword*)desc.bytecode; // Praying that the programmer isnt ever a dork

        // One might be screaming "oh heavens, you never delete them!" and so what? Its like a few kilobytes, relax.
        // The hardware target is 4GB of VRAM, im not developing for the SNES!
        auto res = vkCreateShaderModule(vcore.gpuDevice, &shaderModuleInfo, vcore.allocator, &shader.shader);

        if (!ParseVkResult(res))
        {
            WEngine::WLog::SetConsoleError();
            WEngine::WLog::ConsoleLog(std::format("Unable to create shader \"{}\"!", shader.debugName));
            return 0;
        }

        loadedShaders.push_back(shader);
        return loadedShaders.size();
    }

    ResourceTableLayoutHandle CreateResourceTableLayout(const ResourceTableLayoutDesc& desc)
    {
        Vulkan_ResourceTableLayout layout{};
        layout.debugName = desc.debugName;

        layout.bindings.reserve(desc.entries.size());
        for (const ResourceTableLayoutEntry& entry : desc.entries)
        {
            VkDescriptorSetLayoutBinding binding{};
            binding.binding = entry.binding;
            binding.descriptorType = IrisResourceTableEntryTypeToVulkan(entry.type);
            binding.descriptorCount = entry.count;
            binding.stageFlags = IrisShaderStageToVulkan(entry.stages);
            binding.pImmutableSamplers = nullptr; // samplers are handed out via writes, not baked into the layout
            layout.bindings.push_back(binding);
        }

        VkDescriptorSetLayoutCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        info.bindingCount = layout.bindings.size();
        info.pBindings = layout.bindings.data();

        auto res = vkCreateDescriptorSetLayout(vcore.gpuDevice, &info, vcore.allocator, &layout.layout);

        if (!ParseVkResult(res))
        {
            WEngine::WLog::SetConsoleError();
            WEngine::WLog::ConsoleLog(std::format("Unable to create descriptor set layout \"{}\"!", layout.debugName));
            return 0;
        }
        // Merge pool sizes by descriptor type, so one pool can satisfy the whole layout.
        // (Duplicate pool sizes of the same type are technically summed by the spec, but
        // merging keeps it tidy and avoids any validation shenanigans.)
        for (const VkDescriptorSetLayoutBinding& binding : layout.bindings)
        {
            bool merged = false;
            for (VkDescriptorPoolSize& poolSize : layout.poolSizes)
            {
                if (poolSize.type == binding.descriptorType)
                {
                    poolSize.descriptorCount += binding.descriptorCount;
                    merged = true;
                    break;
                }
            }
            if (!merged)
                layout.poolSizes.push_back({ binding.descriptorType, binding.descriptorCount });
        }

        loadedResourceTableLayouts.push_back(layout);
        return loadedResourceTableLayouts.size();
    }

    ResourceTableHandle CreateResourceTable(ResourceTableLayoutHandle layout)
    {
        if (layout == 0 || layout > loadedResourceTableLayouts.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid resource table layout handle, refusing to create table!");
            return 0;
        }
        const Vulkan_ResourceTableLayout& vkLayout = loadedResourceTableLayouts[layout - 1];

        Vulkan_ResourceTable table{};
        table.debugName = vkLayout.debugName;

        table.layoutHandle = layout;
        // One pool per table. The engine never frees anything, so this is the lazy-but-safe route.
        // Kill me over it, but its supposed to work and not please twitter warriors.
        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.maxSets = 1;
        poolInfo.poolSizeCount = vkLayout.poolSizes.size();
        poolInfo.pPoolSizes = vkLayout.poolSizes.data();

        auto res = vkCreateDescriptorPool(vcore.gpuDevice, &poolInfo, vcore.allocator, &table.pool);

        if (!ParseVkResult(res))
        {
            WEngine::WLog::SetConsoleError();
            WEngine::WLog::ConsoleLog(std::format("Unable to create descriptor pool for table \"{}\"!", table.debugName));
            return 0;
        }
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = table.pool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &vkLayout.layout;
        res = vkAllocateDescriptorSets(vcore.gpuDevice, &allocInfo, &table.set);
        if (!ParseVkResult(res))
        {
            vkDestroyDescriptorPool(vcore.gpuDevice, table.pool, vcore.allocator);
            WEngine::WLog::SetConsoleError();
            WEngine::WLog::ConsoleLog(std::format("Unable to allocate descriptor set for table \"{}\"!", table.debugName));
            return 0;
        }
        loadedResourceTables.push_back(table);
        return loadedResourceTables.size();
    }

    // take a rest before reading this, it's super long.
    GraphicsPipelineHandle CreateGraphicsPipeline(const GraphicsPipelineDesc& desc)
    {
        Vulkan_Pipeline pip{};
        pip.debugName = desc.debugName;

        if (desc.vertexShader == 0 || desc.vertexShader > loadedShaders.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog(std::format("Invalid vertex shader handle, refusing to create pipeline \"{}\"!",
                pip.debugName));
            return 0;
        }
        if (desc.fragmentShader == 0 || desc.fragmentShader > loadedShaders.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog(std::format("Invalid fragment shader handle, refusing to create pipeline \"{}\"!",
                pip.debugName));
            return 0;
        }

        const Vulkan_Shader& vert = loadedShaders[desc.vertexShader - 1];
        const Vulkan_Shader& frag = loadedShaders[desc.fragmentShader - 1];

        if (!(vert.stage & VK_SHADER_STAGE_VERTEX_BIT))
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog(std::format(
                "Provided vertex shader is not a vertex shader, refusing to create pipeline \"{}\"!", pip.debugName));
            return 0;
        }
        if (!(frag.stage & VK_SHADER_STAGE_FRAGMENT_BIT))
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog(std::format(
                "Provided fragment shader is not a fragment shader, refusing to create pipeline \"{}\"!", pip.debugName));
            return 0;
        }

        std::array<VkPushConstantRange, 1> pushConstants;

        pushConstants[0].stageFlags = IrisShaderStageToVulkan(desc.pushConstantsStage);
        pushConstants[0].offset = 0;
        pushConstants[0].size = desc.pushConstantsSize;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.pushConstantRangeCount = pushConstants.size();
        pipelineLayoutInfo.pPushConstantRanges = pushConstants.data();

        wtl::vector<VkDescriptorSetLayout> descriptorSetLayouts(desc.tableAttachmentCount);

        for (sizeT i = 0; i < desc.tableAttachmentCount; i++)
        {
            if (desc.tableLayouts[i] == 0 || desc.tableLayouts[i] > loadedResourceTableLayouts.size())
            {
                WEngine::WLog::SetConsoleWarning();
                WEngine::WLog::ConsoleLog(std::format(
                    "Invalid resource table layout handle, refusing to create pipeline \"{}\"!", pip.debugName));
                return 0;
            }
            const Vulkan_ResourceTableLayout& layout = loadedResourceTableLayouts[desc.tableLayouts[i] - 1];
            descriptorSetLayouts[i] = layout.layout;
        }

        pipelineLayoutInfo.setLayoutCount = descriptorSetLayouts.size();
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

        auto res = vkCreatePipelineLayout(vcore.gpuDevice, &pipelineLayoutInfo, vcore.allocator, &pip.layout);

        if (!ParseVkResult(res))
        {
            WEngine::WLog::SetConsoleError();
            WEngine::WLog::ConsoleLog(std::format("Unable to create pipeline layout for pipeline \"{}\"!", pip.debugName));
            return 0;
        }

        // ----------------------------------------------------------------------

        VkPipelineRasterizationStateCreateInfo rasterInfo{};
        rasterInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterInfo.cullMode = IrisCullModeToVulkan(desc.rasterizer.cullMode);
        rasterInfo.frontFace = IrisFrontFaceToVulkan(desc.rasterizer.frontFace);
        rasterInfo.polygonMode = IrisFillModeToVulkan(desc.rasterizer.fillMode);
        rasterInfo.lineWidth = desc.rasterizer.lineWidth;
        rasterInfo.depthClampEnable = desc.rasterizer.depthClampEnable;
        rasterInfo.depthBiasEnable = desc.rasterizer.depthBiasEnable;
        rasterInfo.depthBiasConstantFactor = desc.rasterizer.depthBiasConstant;
        rasterInfo.depthBiasClamp = desc.rasterizer.depthBiasClamp;
        rasterInfo.depthBiasSlopeFactor = desc.rasterizer.depthBiasSlope;


        VkPipelineColorBlendAttachmentState blendState{};
        blendState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                    VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        if (desc.blend.enableBlending)
        {
            blendState.blendEnable         = VK_TRUE;
            blendState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            blendState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            blendState.colorBlendOp        = VK_BLEND_OP_ADD;
            blendState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            blendState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            blendState.alphaBlendOp        = VK_BLEND_OP_ADD;
        }

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
        if (desc.blend.enableBlending)
            depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        else
            depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        depthStencilInfo.stencilTestEnable = VK_FALSE;
        if (desc.depthStencil.depthTestEnable)
        {
            depthStencilInfo.depthTestEnable = VK_TRUE;
            if (desc.blend.enableBlending)
                depthStencilInfo.depthWriteEnable = VK_FALSE;
            else
                depthStencilInfo.depthWriteEnable = VK_TRUE;
        }

        VkPipelineMultisampleStateCreateInfo multisampleInfo{};
        multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleInfo.rasterizationSamples = IrisSampleCountToVulkan(desc.sampleCount);

        std::array<VkDynamicState, 2> dynamics{};
        dynamics[0] = VK_DYNAMIC_STATE_VIEWPORT;
        dynamics[1] = VK_DYNAMIC_STATE_SCISSOR;

        VkPipelineDynamicStateCreateInfo dynamicInfo{};
        dynamicInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicInfo.dynamicStateCount = dynamics.size();
        dynamicInfo.pDynamicStates = dynamics.data();


        std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};
        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].pName = "main";
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = vert.shader;
        shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].pName = "main";
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module = frag.shader;


        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = IrisTopologyToVulkan(desc.topology);

        wtl::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        wtl::vector<VkVertexInputBindingDescription> bindingDescriptions;

        sizeT offsetCounter = 0;
        for (const auto& attDesc : desc.vertexLayout.attributes)
        {
            VkVertexInputAttributeDescription att{};
            att.format = IrisVertFormatToVulkan(attDesc.format);
            att.location = attDesc.location;
            att.binding = attDesc.binding;
            att.offset = offsetCounter;
            offsetCounter += IrisVertFormatSize(attDesc.format) * sizeof(float32);
            attributeDescriptions.push_back(att);
        }

        for (const auto& bindDesc : desc.vertexLayout.bindings)
        {
            VkVertexInputBindingDescription bind{};
            bind.binding = bindDesc.binding;
            bind.stride = bindDesc.stride;
            if (bindDesc.perInstance)
                bind.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
            else
                bind.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            bindingDescriptions.push_back(bind);
        }

        VkPipelineVertexInputStateCreateInfo vertexInput{};
        vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInput.vertexBindingDescriptionCount = bindingDescriptions.size();
        vertexInput.pVertexBindingDescriptions = bindingDescriptions.data();
        vertexInput.vertexAttributeDescriptionCount = attributeDescriptions.size();
        vertexInput.pVertexAttributeDescriptions = attributeDescriptions.data();

        // This is basically attempting to make the compiler only call it once, we need it in pointer form anyway.
        // No clue if the compiler actually ends up only calling it once thought.
        static VkFormat swapFormat = FindBestSwapchainFormat();

        VkPipelineRenderingCreateInfo dynaRenderInfo{};
        dynaRenderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        dynaRenderInfo.colorAttachmentCount = 1;
        dynaRenderInfo.pColorAttachmentFormats = &swapFormat;
        dynaRenderInfo.depthAttachmentFormat = FindBestDepthFormat(); // hypocrisy

        VkGraphicsPipelineCreateInfo pipeInfo{};
        pipeInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeInfo.pNext = &dynaRenderInfo;
        pipeInfo.pInputAssemblyState = &inputAssembly;
        pipeInfo.pVertexInputState = &vertexInput;
        pipeInfo.pRasterizationState = &rasterInfo;
        pipeInfo.pColorBlendState = &blendInfo;
        pipeInfo.pViewportState = &viewInfo;
        pipeInfo.pDepthStencilState = &depthStencilInfo;
        pipeInfo.pMultisampleState = &multisampleInfo;
        pipeInfo.pDynamicState = &dynamicInfo;
        pipeInfo.stageCount = shaderStages.size();
        pipeInfo.pStages = shaderStages.data();
        pipeInfo.layout = pip.layout;

        res = vkCreateGraphicsPipelines(vcore.gpuDevice, VK_NULL_HANDLE, 1, &pipeInfo, vcore.allocator, &pip.pipeline);

        if (!ParseVkResult(res))
        {
            WEngine::WLog::SetConsoleError();
            WEngine::WLog::ConsoleLog(std::format("Failed to create graphics pipeline \"{}\".", pip.debugName));
            vkDestroyPipelineLayout(vcore.gpuDevice, pipeInfo.layout, vcore.allocator);
            return 0;
        }
        loadedPipelines.push_back(pip);
        return loadedPipelines.size();
    }

    ComputePipelineHandle CreateComputePipeline(const ComputePipelineDesc& desc)
    {
        PrintNotImplemented("CreateComputePipeline");
        return 0;
    }

    FramebufferHandle CreateFramebuffer(const FramebufferDesc& desc)
    {
        PrintNotImplemented("CreateFramebuffer");
        return 0;
    }

    void UpdateBuffer(BufferHandle buffer, sizeT dstOffset, const byte* data, sizeT size)
    {
        if (buffer == 0 || buffer > loadedBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog(std::format("Invalid buffer handle, refusing to Update!"));
            return;
        }
        const Vulkan_Buffer& buff = loadedBuffers[buffer - 1]; // cause handle was gotten by taking size of vector

        sizeT copyExtent = dstOffset + size;
        if (dstOffset > buff.allocInfo.size)
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog(std::format("Unable to copy data into the buffer, as the offset is beyond buffer \"{}\"!",
                buff.debugName));
            return;
        }
        if (copyExtent > buff.allocInfo.size)
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog(std::format("Unable to copy the entire data into buffer \"{}\"!", buff.debugName));
            size = buff.allocInfo.size - dstOffset; // at least to the very end
        }

        memcpy((byte*)buff.allocInfo.pMappedData + dstOffset, data, size);
    }

    void UpdateResourceTable(ResourceTableHandle table, const ResourceTableUpdateDesc& update)
    {
        // vibe coding go brrrr....
        // But yeah, couldnt ever be asked to make all this.
        // Although i think i could have written it a bit cleaner

        if (table == 0 || table > loadedResourceTables.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid resource table handle, refusing to Update!");
            return;
        }

        const Vulkan_ResourceTable& vkTable = loadedResourceTables[table - 1];
        if (vkTable.layoutHandle == 0 || vkTable.layoutHandle > loadedResourceTableLayouts.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog(std::format("Table \"{}\" has an invalid layout handle, refusing to Update!",
                vkTable.debugName));
            return;
        }

        const Vulkan_ResourceTableLayout& vkLayout = loadedResourceTableLayouts[vkTable.layoutHandle - 1];

        // The info structs are only referenced by pointer during vkUpdateDescriptorSets, so they
        // only need to outlive this function. Reserving up front keeps their addresses stable.
        wtl::vector<VkWriteDescriptorSet> writes;
        wtl::vector<VkDescriptorBufferInfo> bufferInfos;
        wtl::vector<VkDescriptorImageInfo> imageInfos;

        writes.reserve(update.writes.size());
        bufferInfos.reserve(update.writes.size());
        imageInfos.reserve(update.writes.size());
        for (const ResourceTableWrite& write : update.writes)
        {
            const VkDescriptorSetLayoutBinding* binding = nullptr;
            for (const VkDescriptorSetLayoutBinding& candidate : vkLayout.bindings)
            {
                if (candidate.binding == write.binding)
                {
                    binding = &candidate;
                    break;
                }
            }
            if (binding == nullptr)
            {
                WEngine::WLog::SetConsoleWarning();
                WEngine::WLog::ConsoleLog(std::format("UpdateResourceTable: no binding {} in layout \"{}\"!",
                    write.binding, vkLayout.debugName));
                continue;
            }
            if (write.arrayIndex >= binding->descriptorCount)
            {
                WEngine::WLog::SetConsoleWarning();
                WEngine::WLog::ConsoleLog(std::format(
                    "UpdateResourceTable: arrayIndex {} out of range for binding {} (count {}) in layout \"{}\"!",
                    write.arrayIndex, write.binding, binding->descriptorCount, vkLayout.debugName));
                continue;
            }

            const VkDescriptorType expectedType = IrisResourceTableEntryTypeToVulkan(write.type);
            if (binding->descriptorType != expectedType)
            {
                WEngine::WLog::SetConsoleWarning();
                WEngine::WLog::ConsoleLog(std::format("UpdateResourceTable: type mismatch on binding {} in layout \"{}\"!",
                    write.binding, vkLayout.debugName));
                continue;
            }

            VkWriteDescriptorSet vkWrite{};
            vkWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            vkWrite.dstSet = vkTable.set;
            vkWrite.dstBinding = write.binding;
            vkWrite.dstArrayElement = write.arrayIndex;
            vkWrite.descriptorCount = 1;
            vkWrite.descriptorType = expectedType;
            switch (write.type)
            {
                case ResourceTableEntryType::UniformBuffer:
                case ResourceTableEntryType::StorageBuffer:
                {
                    if (write.buffer == 0 || write.buffer > loadedBuffers.size())
                    {
                        WEngine::WLog::SetConsoleWarning();
                        WEngine::WLog::ConsoleLog(std::format("UpdateResourceTable: invalid buffer handle on binding {}!",
                            write.binding));
                        continue;
                    }
                    const Vulkan_Buffer& buff = loadedBuffers[write.buffer - 1];
                    // Reserve above guarantees no reallocation, so this reference stays valid.
                    bufferInfos.push_back({});
                    VkDescriptorBufferInfo& info = bufferInfos.back();
                    info.buffer = buff.buffer;
                    info.offset = write.bufferOffset;
                    info.range = write.bufferRange; // max_sizeT == SIZE_MAX == VK_WHOLE_SIZE, which is what we want
                    vkWrite.pBufferInfo = &info;
                    break;
                }
                case ResourceTableEntryType::Texture:
                {
                    if (write.texture == 0 || write.texture > loadedTextures.size() ||
                        write.sampler == 0 || write.sampler > loadedSamplers.size())
                    {
                        WEngine::WLog::SetConsoleWarning();
                        WEngine::WLog::ConsoleLog(std::format("UpdateResourceTable: invalid texture/sampler handle on binding {}!",
                            write.binding));
                        continue;
                    }
                    const Vulkan_Texture& tex = loadedTextures[write.texture - 1];
                    const Vulkan_Sampler& sam = loadedSamplers[write.sampler - 1];
                    imageInfos.push_back({});
                    VkDescriptorImageInfo& info = imageInfos.back();
                    info.sampler = sam.sampler;
                    info.imageView = tex.imageView;
                    info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    vkWrite.pImageInfo = &info;
                    break;
                }
                case ResourceTableEntryType::Sampler:
                {
                    if (write.sampler == 0 || write.sampler > loadedSamplers.size())
                    {
                        WEngine::WLog::SetConsoleWarning();
                        WEngine::WLog::ConsoleLog(std::format("UpdateResourceTable: invalid sampler handle on binding {}!",
                            write.binding));
                        continue;
                    }
                    const Vulkan_Sampler& sam = loadedSamplers[write.sampler - 1];
                    imageInfos.push_back({});
                    VkDescriptorImageInfo& info = imageInfos.back();
                    info.sampler = sam.sampler;
                    info.imageView = VK_NULL_HANDLE;
                    info.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                    vkWrite.pImageInfo = &info;
                    break;
                }
                case ResourceTableEntryType::StorageTexture:
                {
                    if (write.texture == 0 || write.texture > loadedTextures.size())
                    {
                        WEngine::WLog::SetConsoleWarning();
                        WEngine::WLog::ConsoleLog(std::format("UpdateResourceTable: invalid texture handle on binding {}!",
                            write.binding));
                        continue;
                    }
                    const Vulkan_Texture& tex = loadedTextures[write.texture - 1];
                    imageInfos.push_back({});
                    VkDescriptorImageInfo& info = imageInfos.back();
                    info.sampler = VK_NULL_HANDLE;
                    info.imageView = tex.imageView;
                    info.imageLayout = VK_IMAGE_LAYOUT_GENERAL; // storage images must be in GENERAL
                    vkWrite.pImageInfo = &info;
                    break;
                }
            }
            writes.push_back(vkWrite);
        }
        vkUpdateDescriptorSets(vcore.gpuDevice, writes.size(), writes.data(), 0, nullptr);
    }
}

#endif