#if GPU_BACKEND == GPU_VULKAN

#include <Engine/Core/System/Iris.h>

#include "Helpers/Helpers.h"

namespace Iris
{
    BufferHandle CreateBuffer(const BufferDesc& desc)
    {
        PrintNotImplemented("CreateBuffer");
        return 0;
    }

    BufferHandle CreateBuffer(const BufferDesc& desc, const void* initialData, sizeT initialDataSize)
    {
        PrintNotImplemented("CreateBuffer");
        return 0;
    }

    TextureHandle CreateTexture(const TextureDesc& desc)
    {
        PrintNotImplemented("CreateTexture");
        return 0;
    }

    TextureHandle CreateTexture(const TextureDesc& desc, const byte* texData)
    {
        PrintNotImplemented("CreateTexture");
        return 0;
    }

    SamplerHandle CreateSampler(const SamplerDesc& desc)
    {
        PrintNotImplemented("CreateSampler");
        return 0;
    }

    ShaderHandle CreateShader(const ShaderStageDesc& desc)
    {
        PrintNotImplemented("CreateShader");
        return 0;
    }

    ResourceTableLayoutHandle CreateResourceTableLayout(const ResourceTableLayoutDesc& desc)
    {
        PrintNotImplemented("CreateResourceTableLayout");
        return 0;
    }

    ResourceTableHandle CreateResourceTable(ResourceTableLayoutHandle layout)
    {
        PrintNotImplemented("CreateResourceTable");
        return 0;
    }

    GraphicsPipelineHandle CreateGraphicsPipeline(const GraphicsPipelineDesc& desc)
    {
        PrintNotImplemented("CreateGraphicsPipeline");
        return 0;
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
        PrintNotImplemented("UpdateBuffer");
    }

    void UpdateResourceTable(ResourceTableHandle table, const ResourceTableUpdateDesc& update)
    {
        PrintNotImplemented("UpdateResourceTable");
    }
}

#endif