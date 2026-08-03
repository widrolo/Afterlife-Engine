#if GPU_BACKEND == GPU_VULKAN

#include <vk_mem_alloc.h>
#include <Engine/Core/System/Iris.h>

#include "IrisGlobals.h"
#include "Engine/Util/Log.h"
#include "Helpers/Helpers.h"
#include "Helpers/Types.h"

namespace Iris
{
    BufferHandle CreateBuffer(const BufferDesc& desc)
    {
        return CreateBuffer(desc, nullptr, 0);
    }

    BufferHandle CreateBuffer(const BufferDesc& desc, const byte* initialData, sizeT initialDataSize)
    {
        Vulkan_Buffer buff;
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

        if (ParseVkResult(ret))
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
            WEngine::WLog::ConsoleLog(std::format("Unable to copy the entire initial data into buffer \"{}\"!", desc.debugName));
        }

        memcpy(buff.allocInfo.pMappedData, initialData, size);

        return handle;
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
            WEngine::WLog::ConsoleLog(std::format("Unable to copy data into the buffer, as the offset is beyond buffer \"{}\"!", buff.debugName));
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
        PrintNotImplemented("UpdateResourceTable");
    }
}

#endif