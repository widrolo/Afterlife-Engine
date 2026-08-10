#if GPU_BACKEND == GPU_VULKAN

#include "Init.h"

#include "Helpers.h"
#include "Queues.h"
#include "../IrisGlobals.h"
#include "Engine/EngineDefines.h"
#include "Engine/Core/System/GPUSettings.h"
#include "Engine/Core/System/Vulkan/VulkanCore.h"
#include "Engine/Util/Log.h"
#include "Engine/WTL/vector.h"
#include "Game/GameDefines.h"

bool SetupInstance()
{
    const char *validationLayers[] = { "VK_LAYER_KHRONOS_validation" };

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = GameSettings::gameName.c_str();
    appInfo.pEngineName = EngineSettings::engineName.c_str();
    appInfo.apiVersion = GetVulkanVersion();

    wtl::vector<std::string> extensions = GetExtensionsToLoad();
    std::vector<const char*> extensionsData;
    extensionsData.reserve(extensions.size());
    for (const auto& ext : extensions)
        extensionsData.push_back(ext.c_str());

    VkInstanceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pApplicationInfo = &appInfo;
    info.enabledExtensionCount = extensionsData.size();
    info.ppEnabledExtensionNames = extensionsData.data();
    if (GPUSettingsVulkan::enableValidation)
    {
        info.enabledLayerCount = 1;
        info.ppEnabledLayerNames = validationLayers;
    }

    auto resIsnt = vkCreateInstance(&info, vcore.allocator, &vcore.instance);

    return ParseVkResult(resIsnt);
}

bool SetupValidation()
{
    if constexpr (!GPUSettingsVulkan::enableValidation)
        return true;

    auto vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(vcore.instance, "vkCreateDebugUtilsMessengerEXT"));

    if (vkCreateDebugUtilsMessengerEXT == nullptr)
    {
        WEngine::WLog::SetConsoleError();
        WEngine::WLog::ConsoleLog("Failed to load vkCreateDebugUtilsMessengerEXT");
        return false;
    }

    VkDebugUtilsMessengerCreateInfoEXT info{};
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    info.flags = 0;
    info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    info.pfnUserCallback = ValidationCallback;
    info.pUserData = nullptr;

    auto res = vkCreateDebugUtilsMessengerEXT(vcore.instance, &info, vcore.allocator, &vcore.validationMessenger);

    return ParseVkResult(res);
}

bool SetupDevice()
{
    uint32 gpusPresent;
    auto resPhy = vkEnumeratePhysicalDevices(vcore.instance, &gpusPresent, nullptr);

    if (!ParseVkResult(resPhy))
        return false;

    if (gpusPresent < 1)
    {
        WEngine::WLog::SetConsoleError();
        WEngine::WLog::ConsoleLog("[GPU ERROR] No GPU found!");
        return false;
    }

    wtl::vector<VkPhysicalDevice> gpus(gpusPresent);
    auto resPhy2 = vkEnumeratePhysicalDevices(vcore.instance, &gpusPresent, gpus.data());

    if (!ParseVkResult(resPhy2))
        return false;

    for (sizeT i = 0; i < gpusPresent; i++)
    {
        VkPhysicalDeviceProperties2 properties{};
        properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        vkGetPhysicalDeviceProperties2(gpus[i], &properties);

        WEngine::WLog::ConsoleLog(std::format("Found GPU: {}", properties.properties.deviceName));
    }

    vcore.gpuPhysicalDevice = gpus[0];

    VkPhysicalDeviceProperties2 properties{};
    properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    vkGetPhysicalDeviceProperties2(vcore.gpuPhysicalDevice, &properties);

    WEngine::WLog::ConsoleLog(std::format("GPU selected for rendering: {}", properties.properties.deviceName));
    stats.gpuInfo.gpuName = properties.properties.deviceName;

    VkPhysicalDeviceMemoryProperties2 memProps{};
    memProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
    vkGetPhysicalDeviceMemoryProperties2(vcore.gpuPhysicalDevice, &memProps);

    VkDeviceSize totalVram = 0;
    for (sizeT i = 0; i < memProps.memoryProperties.memoryHeapCount; ++i)
    {
        const VkMemoryHeap& heap = memProps.memoryProperties.memoryHeaps[i];
        if (heap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
            totalVram += heap.size;
    }
    stats.gpuInfo.totalVram = totalVram;

    auto queues = FindDeviceQueues();

    wtl::vector<const char*> extensions = {
        "VK_KHR_swapchain", "VK_KHR_dynamic_rendering"
    };


    VkPhysicalDeviceDynamicRenderingFeatures dynaRendering{};
    dynaRendering.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
    dynaRendering.dynamicRendering = VK_TRUE;

    VkDeviceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.pNext = &dynaRendering;
    info.queueCreateInfoCount = queues.size();
    info.pQueueCreateInfos = queues.data();
    info.enabledExtensionCount = extensions.size();
    info.ppEnabledExtensionNames = extensions.data();

    // Do I also have to hire a babysitter for the damn gpu??
    auto resDev = vkCreateDevice(vcore.gpuPhysicalDevice, &info, vcore.allocator, &vcore.gpuDevice);

    if (!ParseVkResult(resDev))
        return false;

    SetupDeviceQueues();

    for (auto queue : queues)
        wFree((void*)queue.pQueuePriorities);

    return true;
}

#endif

