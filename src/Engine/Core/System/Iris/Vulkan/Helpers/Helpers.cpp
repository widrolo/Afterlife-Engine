#if GPU_BACKEND == GPU_VULKAN
#include "Helpers.h"

#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

#include "Engine/Core/System/GPUSettings.h"
#include "Engine/Core/System/Iris/Vulkan/IrisGlobals.h"
#include "SDL3/SDL_vulkan.h"

#include "Engine/Util/Log.h"

void PrintNotImplemented(const std::string &functionName)
{
    WEngine::WLog::SetConsoleWarning();
    WEngine::WLog::ConsoleLog(std::format("Iris Error: Function \"{}\" not yet implemented!", functionName));
}

bool ParseVkResult(VkResult result)
{
    if (result == VK_SUCCESS)
        return true;

    WEngine::WLog::SetConsoleError();
    WEngine::WLog::ConsoleLog(std::format("[GPU ERROR] VkResult was not success! {}", string_VkResult(result)));

    switch (GPUSettingsVulkan::invalidResultAction)
    {
        case InvalidResultAction::LetGo:
            return false;
        case InvalidResultAction::Stall:
            while (true);
        case InvalidResultAction::Abort:
            abort();
        default:
            return false;
    }
}

VkBool32 ValidationCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*)
{
    if (pCallbackData->pMessage == nullptr)
        return VK_FALSE;

    std::string warnStart = "[GPU ";

    if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        warnStart += "Warning]";
        WEngine::WLog::SetConsoleWarning();
    }
    else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        warnStart += "Error]";
        WEngine::WLog::SetConsoleError();
    }
    else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    {
        warnStart += "Info]";
        WEngine::WLog::SetConsoleInfo();
    }

    if (messageTypes == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
        WEngine::WLog::ConsoleLog(std::format("{} Validation layer tripped! \"{}\"\n{}", warnStart, pCallbackData->pMessageIdName, pCallbackData->pMessage));
    else if (messageTypes == VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
        WEngine::WLog::ConsoleLog(std::format("{} Non-optimal use of Vulkan! \"{}\"\n{}", warnStart, pCallbackData->pMessageIdName, pCallbackData->pMessage));

    if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        switch (GPUSettingsVulkan::validationErrorAction)
        {
            case InvalidResultAction::LetGo:
                return VK_FALSE;
            case InvalidResultAction::Stall:
                while (true);
            case InvalidResultAction::Abort:
                abort();
            default:
                return VK_FALSE;
        }
    }

    return VK_FALSE;
}

void PerformInvalidHandleAction()
{
    switch (GPUSettings::invalidHandleAction)
    {
        case InvalidResultAction::LetGo:
            return;
        case InvalidResultAction::Stall:
            while (true);
        case InvalidResultAction::Abort:
            abort();
        default:
            return;
    }
}

uint64 CalcTextureSize(uint8 bytesPerPixel, uint32 width, uint32 height)
{
    return width * height * bytesPerPixel;
}

sizeT BCBlockSize(BC format)
{
    return (format == BC::BC5) ? 16u : 8u;
}

sizeT BCMipSize(uint32 width, uint32 height, BC format)
{
    uint32_t bw = std::max(1u, (width + 3) / 4);
    uint32_t bh = std::max(1u, (height + 3) / 4);
    return bw * bh * BCBlockSize(format);
}

uint64 CalcModelSize(uint8 bytesPerVertex, uint32 vertexCount)
{
    return bytesPerVertex * vertexCount;
}

uint64 GetSizeOfImageInBytes(WEngine::Vector2 imageSize, uint8 channelCount)
{
    return (uint64)imageSize.x * (uint64)imageSize.y * channelCount;
}

uint32 GetVulkanVersion()
{
    return VK_API_VERSION_1_4;
}

VkBufferUsageFlags IrisBuffUsageToVulkan(Iris::BufferUsage usage)
{
    uint32 flags = 0;

    if ((uint8)usage & (uint8)Iris::BufferUsage::Vertex)
        flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    if ((uint8)usage & (uint8)Iris::BufferUsage::Index)
        flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    if ((uint8)usage & (uint8)Iris::BufferUsage::Uniform)
        flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    if ((uint8)usage & (uint8)Iris::BufferUsage::Storage)
        flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    if ((uint8)usage & (uint8)Iris::BufferUsage::TransferSrc)
        flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    if ((uint8)usage & (uint8)Iris::BufferUsage::TransferDst)
        flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    return (VkBufferUsageFlags)flags;
}

VkFormat IrisImgFormatToVulkan(Iris::ImgFormat format)
{
    switch (format)
    {
        case Iris::ImgFormat::Unknown:              return VK_FORMAT_UNDEFINED;
        case Iris::ImgFormat::RGBA8_UNorm:          return VK_FORMAT_R8G8B8A8_UNORM;
        case Iris::ImgFormat::BGRA8_UNorm:          return VK_FORMAT_B8G8R8A8_UNORM;
        case Iris::ImgFormat::RGBA8_SRGB:           return VK_FORMAT_R8G8B8A8_SRGB;
        case Iris::ImgFormat::BGRA8_SRGB:           return VK_FORMAT_B8G8R8A8_SRGB;
        case Iris::ImgFormat::D32_SFloat_S8_UInt:   return VK_FORMAT_D32_SFLOAT_S8_UINT;
        case Iris::ImgFormat::BC1:                  return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
        case Iris::ImgFormat::BC4:                  return VK_FORMAT_BC4_UNORM_BLOCK;
        case Iris::ImgFormat::BC5:                  return VK_FORMAT_BC5_UNORM_BLOCK;
        default: return VK_FORMAT_UNDEFINED;
    }
}

VkFilter IrisFilterToVulkan(Iris::FilterMode filter)
{
    switch (filter)
    {
        case Iris::FilterMode::Nearest:     return VK_FILTER_NEAREST;
        case Iris::FilterMode::Linear:      return VK_FILTER_LINEAR;
        default: return VK_FILTER_NEAREST;
    }
}
VkSamplerMipmapMode IrisMipFilterToVulkan(Iris::MipFilterMode mipFilter)
{
    switch (mipFilter)
    {
        // Vulkan has no "no mipmap" mode; None is handled at sampler-creation
        // time by setting maxLod = 0.0f, so map it to NEAREST here.
        case Iris::MipFilterMode::None:     return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        case Iris::MipFilterMode::Nearest:  return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        case Iris::MipFilterMode::Linear:   return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        default: return VK_SAMPLER_MIPMAP_MODE_NEAREST;
    }
}
VkSamplerAddressMode IrisSamAddrModeToVulkan(Iris::SamplerAddressMode mode)
{
    switch (mode)
    {
        case Iris::SamplerAddressMode::Repeat:          return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case Iris::SamplerAddressMode::MirrorRepeat:    return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case Iris::SamplerAddressMode::ClampToEdge:     return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case Iris::SamplerAddressMode::ClampToBorder:   return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        default: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    }
}

VkCompareOp IrisCompareOpToVulkan(Iris::CompareOp op)
{
    switch (op)
    {
        case Iris::CompareOp::Never:        return VK_COMPARE_OP_NEVER;
        case Iris::CompareOp::Less:         return VK_COMPARE_OP_LESS;
        case Iris::CompareOp::Equal:        return VK_COMPARE_OP_EQUAL;
        case Iris::CompareOp::LessEqual:    return VK_COMPARE_OP_LESS_OR_EQUAL;
        case Iris::CompareOp::Greater:      return VK_COMPARE_OP_GREATER;
        case Iris::CompareOp::NotEqual:     return VK_COMPARE_OP_NOT_EQUAL;
        case Iris::CompareOp::GreaterEqual: return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case Iris::CompareOp::Always:       return VK_COMPARE_OP_ALWAYS;
        default: return VK_COMPARE_OP_NEVER;
    }
}
VkCullModeFlags IrisCullModeToVulkan(Iris::CullMode mode)
{
    switch (mode)
    {
        case Iris::CullMode::None:  return VK_CULL_MODE_NONE;
        case Iris::CullMode::Front: return VK_CULL_MODE_FRONT_BIT;
        case Iris::CullMode::Back:  return VK_CULL_MODE_BACK_BIT;
        default: return VK_CULL_MODE_NONE;
    }
}
VkFrontFace IrisFrontFaceToVulkan(Iris::FrontFace frontFace)
{
    switch (frontFace)
    {
        case Iris::FrontFace::CounterClockwise: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        case Iris::FrontFace::Clockwise:        return VK_FRONT_FACE_CLOCKWISE;
        default: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    }
}
VkPolygonMode IrisFillModeToVulkan(Iris::FillMode fillMode)
{
    switch (fillMode)
    {
        case Iris::FillMode::Solid:     return VK_POLYGON_MODE_FILL;
        case Iris::FillMode::Wireframe: return VK_POLYGON_MODE_LINE;
        default: return VK_POLYGON_MODE_FILL;
    }
}
VkBlendFactor IrisBlendFactorToVulkan(Iris::BlendFactor factor)
{
    switch (factor)
    {
        case Iris::BlendFactor::Zero:                    return VK_BLEND_FACTOR_ZERO;
        case Iris::BlendFactor::One:                     return VK_BLEND_FACTOR_ONE;
        case Iris::BlendFactor::SrcColor:                return VK_BLEND_FACTOR_SRC_COLOR;
        case Iris::BlendFactor::OneMinusSrcColor:        return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case Iris::BlendFactor::DstColor:                return VK_BLEND_FACTOR_DST_COLOR;
        case Iris::BlendFactor::OneMinusDstColor:        return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case Iris::BlendFactor::SrcAlpha:                return VK_BLEND_FACTOR_SRC_ALPHA;
        case Iris::BlendFactor::OneMinusSrcAlpha:        return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case Iris::BlendFactor::DstAlpha:                return VK_BLEND_FACTOR_DST_ALPHA;
        case Iris::BlendFactor::OneMinusDstAlpha:        return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case Iris::BlendFactor::ConstantColor:           return VK_BLEND_FACTOR_CONSTANT_COLOR;
        case Iris::BlendFactor::OneMinusConstantColor:   return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
        case Iris::BlendFactor::ConstantAlpha:           return VK_BLEND_FACTOR_CONSTANT_ALPHA;
        case Iris::BlendFactor::OneMinusConstantAlpha:   return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
        case Iris::BlendFactor::SrcAlphaSaturate:        return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
        case Iris::BlendFactor::Src1Color:               return VK_BLEND_FACTOR_SRC1_COLOR;
        case Iris::BlendFactor::OneMinusSrc1Color:       return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
        case Iris::BlendFactor::Src1Alpha:               return VK_BLEND_FACTOR_SRC1_ALPHA;
        case Iris::BlendFactor::OneMinusSrc1Alpha:       return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
        default: return VK_BLEND_FACTOR_ONE;
    }
}
VkBlendOp IrisBlendOpToVulkan(Iris::BlendOp op)
{
    switch (op)
    {
        case Iris::BlendOp::Add:            return VK_BLEND_OP_ADD;
        case Iris::BlendOp::Subtract:       return VK_BLEND_OP_SUBTRACT;
        case Iris::BlendOp::ReverseSubtract: return VK_BLEND_OP_REVERSE_SUBTRACT;
        case Iris::BlendOp::Min:            return VK_BLEND_OP_MIN;
        case Iris::BlendOp::Max:            return VK_BLEND_OP_MAX;
        default: return VK_BLEND_OP_ADD;
    }
}
VkStencilOp IrisStencilOpToVulkan(Iris::StencilOp op)
{
    switch (op)
    {
        case Iris::StencilOp::Keep:            return VK_STENCIL_OP_KEEP;
        case Iris::StencilOp::Zero:            return VK_STENCIL_OP_ZERO;
        case Iris::StencilOp::Replace:         return VK_STENCIL_OP_REPLACE;
        case Iris::StencilOp::IncrementClamp:  return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
        case Iris::StencilOp::DecrementClamp:  return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
        case Iris::StencilOp::IncrementWrap:   return VK_STENCIL_OP_INCREMENT_AND_WRAP;
        case Iris::StencilOp::DecrementWrap:   return VK_STENCIL_OP_DECREMENT_AND_WRAP;
        case Iris::StencilOp::Invert:          return VK_STENCIL_OP_INVERT;
        default: return VK_STENCIL_OP_KEEP;
    }
}
VkLogicOp IrisLogicOpToVulkan(Iris::LogicOp op)
{
    switch (op)
    {
        case Iris::LogicOp::Clear:         return VK_LOGIC_OP_CLEAR;
        case Iris::LogicOp::Set:           return VK_LOGIC_OP_SET;
        case Iris::LogicOp::Copy:          return VK_LOGIC_OP_COPY;
        case Iris::LogicOp::CopyInverted:  return VK_LOGIC_OP_COPY_INVERTED;
        case Iris::LogicOp::Noop:          return VK_LOGIC_OP_NO_OP;
        case Iris::LogicOp::Inverted:      return VK_LOGIC_OP_INVERT;
        case Iris::LogicOp::And:           return VK_LOGIC_OP_AND;
        case Iris::LogicOp::Nand:          return VK_LOGIC_OP_NAND;
        case Iris::LogicOp::Or:            return VK_LOGIC_OP_OR;
        case Iris::LogicOp::Nor:           return VK_LOGIC_OP_NOR;
        case Iris::LogicOp::Xor:           return VK_LOGIC_OP_XOR;
        case Iris::LogicOp::Equiv:         return VK_LOGIC_OP_EQUIVALENT;
        case Iris::LogicOp::AndReverse:    return VK_LOGIC_OP_AND_REVERSE;
        case Iris::LogicOp::AndInverted:   return VK_LOGIC_OP_AND_INVERTED;
        case Iris::LogicOp::OrReverse:     return VK_LOGIC_OP_OR_REVERSE;
        case Iris::LogicOp::OrInverted:    return VK_LOGIC_OP_OR_INVERTED;
        default: return VK_LOGIC_OP_COPY;
    }
}

VkBorderColor IrisBorderColorToVulkan(Iris::BorderColor color)
{
    switch (color)
    {
        case Iris::BorderColor::TransparentBlack: return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        case Iris::BorderColor::OpaqueBlack:      return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        case Iris::BorderColor::OpaqueWhite:      return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        default: return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    }
}

VkAttachmentLoadOp IrisLoadOpToVulkan(Iris::LoadOp op)
{
    switch (op)
    {
        case Iris::LoadOp::Load:     return VK_ATTACHMENT_LOAD_OP_LOAD;
        case Iris::LoadOp::Clear:    return VK_ATTACHMENT_LOAD_OP_CLEAR;
        case Iris::LoadOp::DontCare: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        default: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    }
}

VkAttachmentStoreOp IrisStoreOpToVulkan(Iris::StoreOp op)
{
    switch (op)
    {
        case Iris::StoreOp::Store:    return VK_ATTACHMENT_STORE_OP_STORE;
        case Iris::StoreOp::DontCare: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
        default: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
    }
}

VkShaderStageFlags IrisShaderStageToVulkan(Iris::ShaderStage stage)
{
    VkShaderStageFlags flags = 0;
    if ((uint8)stage & (uint8)Iris::ShaderStage::Vertex)
        flags |= VK_SHADER_STAGE_VERTEX_BIT;
    if ((uint8)stage & (uint8)Iris::ShaderStage::Fragment)
        flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
    if ((uint8)stage & (uint8)Iris::ShaderStage::Compute)
        flags |= VK_SHADER_STAGE_COMPUTE_BIT;
    if ((uint8)stage & (uint8)Iris::ShaderStage::Geometry)
        flags |= VK_SHADER_STAGE_GEOMETRY_BIT;
    if ((uint8)stage & (uint8)Iris::ShaderStage::TessControl)
        flags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    if ((uint8)stage & (uint8)Iris::ShaderStage::TessEval)
        flags |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    return (VkShaderStageFlags)flags;
}
VkSampleCountFlagBits IrisSampleCountToVulkan(Iris::SampleCount count)
{
    switch (count)
    {
        case Iris::SampleCount::Samples1: return VK_SAMPLE_COUNT_1_BIT;
        case Iris::SampleCount::Samples2: return VK_SAMPLE_COUNT_2_BIT;
        case Iris::SampleCount::Samples4: return VK_SAMPLE_COUNT_4_BIT;
        case Iris::SampleCount::Samples8: return VK_SAMPLE_COUNT_8_BIT;
        default: return VK_SAMPLE_COUNT_1_BIT;
    }
}
VkImageUsageFlags IrisTextureUsageToVulkan(Iris::TextureUsage usage)
{
    VkImageUsageFlags flags = 0;
    if ((uint8)usage & (uint8)Iris::TextureUsage::Sampled)
        flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
    if ((uint8)usage & (uint8)Iris::TextureUsage::Storage)
        flags |= VK_IMAGE_USAGE_STORAGE_BIT;
    if ((uint8)usage & (uint8)Iris::TextureUsage::ColorTarget)
        flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if ((uint8)usage & (uint8)Iris::TextureUsage::DepthTarget)
        flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    if ((uint8)usage & (uint8)Iris::TextureUsage::TransferSrc)
        flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    if ((uint8)usage & (uint8)Iris::TextureUsage::TransferDst)
        flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    return (VkImageUsageFlags)flags;
}

VkDescriptorType IrisResourceTableEntryTypeToVulkan(Iris::ResourceTableEntryType type)
{
    switch (type)
    {
        case Iris::ResourceTableEntryType::UniformBuffer:  return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case Iris::ResourceTableEntryType::StorageBuffer:  return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        case Iris::ResourceTableEntryType::Texture:        return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        case Iris::ResourceTableEntryType::Sampler:        return VK_DESCRIPTOR_TYPE_SAMPLER;
        case Iris::ResourceTableEntryType::StorageTexture: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        default: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }
}

VkPrimitiveTopology IrisTopologyToVulkan(Iris::TopologyType topology)
{
    switch (topology)
    {
        case Iris::TopologyType::Point_List:      return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        case Iris::TopologyType::Line_List:       return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        case Iris::TopologyType::Line_Strip:      return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        case Iris::TopologyType::Triangle_List:   return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        case Iris::TopologyType::Triangle_Strip:  return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        case Iris::TopologyType::Triangle_Fan:    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
        default:                                  return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }
}

VkFormat IrisVertFormatToVulkan(Iris::VertFormat format)
{
    switch (format)
    {
        case Iris::VertFormat::Float1: return VK_FORMAT_R32_SFLOAT;
        case Iris::VertFormat::Float2: return VK_FORMAT_R32G32_SFLOAT;
        case Iris::VertFormat::Float3: return VK_FORMAT_R32G32B32_SFLOAT;
        case Iris::VertFormat::Float4: return VK_FORMAT_R32G32B32A32_SFLOAT;
        default:                       return VK_FORMAT_UNDEFINED;
    }
}

sizeT IrisVertFormatSize(Iris::VertFormat format)
{
    switch (format)
    {
        case Iris::VertFormat::Float1: return 1;
        case Iris::VertFormat::Float2: return 2;
        case Iris::VertFormat::Float3: return 3;
        case Iris::VertFormat::Float4: return 4;
        default:                       return 0;
    }
}

VkFormat FindBestSwapchainFormat()
{
    wtl::vector<VkFormat> candidates =
    {
        VK_FORMAT_B8G8R8A8_UNORM,
        VK_FORMAT_R8G8B8A8_UNORM
    };

    for (VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(vcore.gpuPhysicalDevice, format, &props);

        if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT)
            return format;
    }

    return VK_FORMAT_UNDEFINED;
}

VkFormat FindBestDepthFormat()
{
    wtl::vector<VkFormat> candidates =
    {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };

    for (VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(vcore.gpuPhysicalDevice, format, &props);

        if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
            return format;
    }

    return VK_FORMAT_UNDEFINED;
}

VkCommandBuffer GetCurrentCmdBuff(Iris::CommandBufferHandle cmd)
{
    const uint32 slot = commandBufferFrameIndex % (uint32)framePools.size();
    return loadedCommandBuffers[cmd - 1].commandBuffers[slot];
}

wtl::vector<std::string> GetExtensionsToLoad()
{
    wtl::vector<std::string> extensions = {VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
    uint32 count_instance_extensions;
    const char * const *instance_extensions = SDL_Vulkan_GetInstanceExtensions(&count_instance_extensions);

    if (instance_extensions != nullptr)
    {
#ifdef WE_Windows
        int count_extensions = count_instance_extensions;
        for (int i = 0; i < count_extensions; i++)
            extensions.emplace_back(instance_extensions[i]);
#else
        int count_extensions = count_instance_extensions + 1;
        for (int i = 1; i < count_extensions; i++)
            extensions.emplace_back(instance_extensions[i]);
#endif


    }

    return extensions;
}

#endif
