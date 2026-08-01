#pragma once
#include <string>

#include "Handles.h"
#include "Usage.h"
#include "Engine/WTL/vector.h"

namespace Iris
{
    enum class ResourceTableEntryType : uint8
    {
        UniformBuffer,
        StorageBuffer,
        Texture,
        Sampler,
        StorageTexture,
    };

    struct ResourceTableLayoutEntry
    {
        uint32 binding;
        ShaderStage stages;
        ResourceTableEntryType type;
        uint32 count = 1;
    };

    struct ResourceTableLayoutDesc
    {
        wtl::vector<ResourceTableLayoutEntry> entries;
        std::string debugName;
    };

    struct ResourceTableWrite
    {
        uint32 binding;
        uint32 arrayIndex = 0;
        ResourceTableEntryType type;
        BufferHandle buffer;
        sizeT bufferOffset = 0;
        sizeT bufferRange = max_sizeT;
        TextureHandle texture;
        SamplerHandle sampler;
    };

    struct ResourceTableUpdateDesc
    {
        wtl::vector<ResourceTableWrite> writes;
    };
}
