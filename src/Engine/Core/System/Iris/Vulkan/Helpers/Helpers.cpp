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
