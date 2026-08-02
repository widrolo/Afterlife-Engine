#include <Engine/EngineDefines.h>
//#if GPU_BACKEND == GPU_VULKAN
#ifdef ehbwfaj

#include "VulkanHelpers.h"
#include <Engine/Core/System/GPUSettings.h>
#include "Engine/Util/Log.h"
#include "Engine/WTL/vector.h"
#include <vulkan/vk_enum_string_helper.h>
#include <vk_mem_alloc.h>


uint32 GetVulkanVersionabfj()
{
    return VK_API_VERSION_1_4;
}

WEngine::Vector2 & GetFbResolution(const VulkanContext &ctx)
{
    return ctx.currentRenderTarget->resolution;
}

VkCommandBuffer& GetFbCmdBuff(const VulkanContext &ctx)
{
    return ctx.currentRenderTarget->cmdBuffs[ctx.screen.currentFrame];
}

VkImage& GetFbImage(const VulkanContext &ctx)
{
    bool isSwapchainImage = ctx.currentRenderTarget == &ctx.displayTarget;
    return isSwapchainImage ? ctx.currentRenderTarget->targetImages[ctx.screen.swapchainCurrentImage] :
        ctx.currentRenderTarget->targetImages[ctx.screen.currentFrame];
}

VkImage& GetFbImage(const VulkanContext &ctx, Vulkan_RenderTarget &rt)
{
    bool isSwapchainImage = &rt == &ctx.displayTarget;
    return isSwapchainImage ? rt.targetImages[ctx.screen.swapchainCurrentImage] :
        rt.targetImages[ctx.screen.currentFrame];
}

VkImageView& GetFbImageView(const VulkanContext &ctx)
{
    bool isSwapchainImage = ctx.currentRenderTarget == &ctx.displayTarget;
    return isSwapchainImage ? ctx.currentRenderTarget->targetImageViews[ctx.screen.swapchainCurrentImage] :
        ctx.currentRenderTarget->targetImageViews[ctx.screen.currentFrame];
}

VkImageView& GetFbImageView(const VulkanContext &ctx, Vulkan_RenderTarget &rt)
{
    bool isSwapchainImage = &rt == &ctx.displayTarget;
    return isSwapchainImage ? rt.targetImageViews[ctx.screen.swapchainCurrentImage] :
        rt.targetImageViews[ctx.screen.currentFrame];
}

VkImage& GetFbDepthImage(const VulkanContext &ctx)
{
    bool isSwapchainImage = ctx.currentRenderTarget == &ctx.displayTarget;
    return isSwapchainImage ? ctx.currentRenderTarget->depthImages[ctx.screen.swapchainCurrentImage] :
        ctx.currentRenderTarget->depthImages[ctx.screen.currentFrame];
}

VkImage& GetFbDepthImage(const VulkanContext &ctx, Vulkan_RenderTarget &rt)
{
    bool isSwapchainImage = &rt == &ctx.displayTarget;
    return isSwapchainImage ? rt.depthImages[ctx.screen.swapchainCurrentImage] :
        rt.depthImages[ctx.screen.currentFrame];
}

VkImageView& GetFbDepthImageView(const VulkanContext &ctx)
{
    bool isSwapchainImage = ctx.currentRenderTarget == &ctx.displayTarget;
    return isSwapchainImage ? ctx.currentRenderTarget->depthImageViews[ctx.screen.swapchainCurrentImage] :
        ctx.currentRenderTarget->depthImageViews[ctx.screen.currentFrame];
}

VkImageView& GetFbDepthImageView(const VulkanContext &ctx, Vulkan_RenderTarget &rt)
{
    bool isSwapchainImage = &rt == &ctx.displayTarget;
    return isSwapchainImage ? rt.depthImageViews[ctx.screen.swapchainCurrentImage] :
        rt.depthImageViews[ctx.screen.currentFrame];
}

VkSemaphore& GetFbImageAvailSem(const VulkanContext &ctx)
{
    return ctx.currentRenderTarget->imageAvailableSems[ctx.screen.currentFrame];
}

VkSemaphore& GetFbRenderFinishedSem(const VulkanContext &ctx)
{
    bool isSwapchainImage = ctx.currentRenderTarget == &ctx.displayTarget;
    return isSwapchainImage ? ctx.currentRenderTarget->renderFinishedSems[ctx.screen.swapchainCurrentImage] :
        ctx.currentRenderTarget->renderFinishedSems[ctx.screen.currentFrame];
}

VkFence& GetFbEndOfFrameFence(const VulkanContext &ctx)
{
    return ctx.currentRenderTarget->endOfFrameFences[ctx.screen.currentFrame];
}

VkDescriptorSet& GetFbDescriptorSet(const VulkanContext &ctx, Vulkan_RenderTarget &rt)
{
    bool isSwapchainImage = &rt == &ctx.displayTarget;
    return isSwapchainImage ? rt.descSets[ctx.screen.swapchainCurrentImage] :
        rt.descSets[ctx.screen.currentFrame];
}

VkImageLayout& GetFbLayout(const VulkanContext &ctx)
{
    bool isSwapchainImage = ctx.currentRenderTarget == &ctx.displayTarget;
    return isSwapchainImage ? ctx.currentRenderTarget->currentLayouts[ctx.screen.swapchainCurrentImage] :
        ctx.currentRenderTarget->currentLayouts[ctx.screen.currentFrame];
}

VkImageLayout& GetFbLayout(const VulkanContext &ctx, Vulkan_RenderTarget &rt)
{
    bool isSwapchainImage = &rt == &ctx.displayTarget;
    return isSwapchainImage ? rt.currentLayouts[ctx.screen.swapchainCurrentImage] :
        rt.currentLayouts[ctx.screen.currentFrame];
}

VkImageLayout& GetFbDepthLayout(const VulkanContext &ctx)
{
    bool isSwapchainImage = ctx.currentRenderTarget == &ctx.displayTarget;
    return isSwapchainImage ? ctx.currentRenderTarget->currentDepthLayouts[ctx.screen.swapchainCurrentImage] :
        ctx.currentRenderTarget->currentLayouts[ctx.screen.currentFrame];
}

VkImageLayout& GetFbDepthLayout(const VulkanContext &ctx, Vulkan_RenderTarget &rt)
{
    bool isSwapchainImage = &rt == &ctx.displayTarget;
    return isSwapchainImage ? rt.currentDepthLayouts[ctx.screen.swapchainCurrentImage] :
        rt.currentLayouts[ctx.screen.currentFrame];
}

bool GetFbDepthAvail(const VulkanContext &ctx)
{
    return ctx.currentRenderTarget->hasDepth;
}

bool GetFbDepthAvail(const VulkanContext &ctx, Vulkan_RenderTarget &rt)
{
    return rt.hasDepth;
}

Vulkan_StatBuf& GetStatBuf(VulkanContext &ctx, WEngine::StatBufKey key)
{
    return ctx.statBuffers[key - 1];
}

Vulkan_Material& GetLoadedMaterial(VulkanContext &ctx, WEngine::Material material)
{
    if (material == 0 || material > ctx.loadedMaterials.size())
    {
        WEngine::WLog::SetConsoleWarning();
        WEngine::WLog::ConsoleLog("Tried to get a material with invalid handle!");
        PerformInvalidHandleAction();
    }
    return ctx.loadedMaterials[material - 1];
}

Vulkan_Shader& GetLoadedShader(VulkanContext &ctx, WEngine::Shader shader)
{
    if (shader == 0 || shader > ctx.loadedShaders.size())
    {
        WEngine::WLog::SetConsoleWarning();
        WEngine::WLog::ConsoleLog("Tried to get a shader with invalid handle!");
        PerformInvalidHandleAction();
    }
    return ctx.loadedShaders[shader - 1];
}

Vulkan_Model& GetLoadedModel(VulkanContext &ctx, WEngine::Model model)
{
    if (model == 0 || model > ctx.loadedModels.size())
    {
        WEngine::WLog::SetConsoleWarning();
        WEngine::WLog::ConsoleLog("Tried to get a model with invalid handle!");
        PerformInvalidHandleAction();
    }
    return ctx.loadedModels[model - 1];
}

Vulkan_RenderTarget& GetLoadedRenderTarget(VulkanContext &ctx, WEngine::Framebuffer fb)
{
    if (fb == 0 || fb > ctx.renderTargets.size())
    {
        WEngine::WLog::SetConsoleWarning();
        WEngine::WLog::ConsoleLog("Tried to get a framebuffer with invalid handle!");
        PerformInvalidHandleAction();
    }
    return ctx.renderTargets[fb - 1];
}

bool AddExtensionIfAvailable(VulkanContext &ctx, wtl::vector<const char*> &destination, const char* extensionName)
{
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(ctx.vcore.gpuPhysicalDevice, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(ctx.vcore.gpuPhysicalDevice, nullptr, &extensionCount, availableExtensions.data());

    bool isAvailable = std::any_of(
        availableExtensions.begin(),
        availableExtensions.end(),
        [&extensionName](const VkExtensionProperties &ext)
        {
            return std::strcmp(extensionName, ext.extensionName) == 0;
        }
    );

    if (!isAvailable)
    {
        WEngine::WLog::SetConsoleWarning();
        WEngine::WLog::ConsoleLog(std::format("Extension {} unavailable, praying there is a fallback.", extensionName));
        return false;
    }

    destination.push_back(extensionName);
    return true;
}
void PopulatePushConstants(const VulkanContext &ctx, const Vulkan_Shader &shader, const WEngine::Mat4x4 &mvp)
{
    vkCmdPushConstants(GetFbCmdBuff(ctx), shader.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0,
        sizeof(WEngine::Mat4x4), &mvp);
}

#endif
