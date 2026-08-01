#if GPU_BACKEND == GPU_VULKAN

#include <Engine/Core/System/Iris.h>

using namespace Iris;

BufferHandle CreateBuffer(const BufferDesc& desc)
{

}

BufferHandle CreateBuffer(const BufferDesc& desc, const void* initialData, sizeT initialDataSize)
{

}

TextureHandle CreateTexture(const TextureDesc& desc)
{

}

TextureHandle CreateTexture(const TextureDesc& desc, const byte* texData)
{

}

SamplerHandle CreateSampler(const SamplerDesc& desc)
{

}

ShaderHandle CreateShader(const ShaderStageDesc& desc)
{

}

ResourceTableLayoutHandle CreateResourceTableLayout(const ResourceTableLayoutDesc& desc)
{

}

ResourceTableHandle CreateResourceTable(ResourceTableLayoutHandle layout)
{

}

GraphicsPipelineHandle CreateGraphicsPipeline(const GraphicsPipelineDesc& desc)
{

}

ComputePipelineHandle CreateComputePipeline(const ComputePipelineDesc& desc)
{

}

FramebufferHandle CreateFramebuffer(const FramebufferDesc& desc)
{

}

void UpdateBuffer(BufferHandle buffer, sizeT dstOffset, const byte* data, sizeT size)
{

}

void UpdateResourceTable(ResourceTableHandle table, const ResourceTableUpdateDesc& update)
{

}

#endif