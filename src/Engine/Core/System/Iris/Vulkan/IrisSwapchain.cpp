#if GPU_BACKEND == GPU_VULKAN

#include <Engine/Core/System/Iris.h>

#include "IrisGlobals.h"
#include "Engine/Util/Log.h"
#include "Helpers/Helpers.h"

namespace Iris
{
    TextureHandle AcquireSwapchainTexture()
    {
        uint32 imageIndex = 0;
        VkResult res = vkAcquireNextImageKHR(vcore.gpuDevice, screen.swapchain, UINT64_MAX,
                                             displayTarget.imageAvailableSems[screen.currentFrame],
                                             VK_NULL_HANDLE, &imageIndex);
        if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR)
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Swapchain out of date, resize handling not implemented yet.");
            return 0;
        }
        screen.swapchainCurrentImage = imageIndex;
        return swapchainTextureHandles[imageIndex];
    }

    FramebufferHandle GetSwapchainFramebuffer()
    {
        PrintNotImplemented("GetSwapchainFramebuffer");
        return 0;
    }

    void Present()
    {
        VkResult renderRes;
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &displayTarget.renderFinishedSems[screen.currentFrame];
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &screen.swapchain;
        presentInfo.pImageIndices = &screen.swapchainCurrentImage;
        presentInfo.pResults = &renderRes;

        vkQueuePresentKHR(queues.primaryDrawQueue, &presentInfo);

        screen.currentFrame = (screen.currentFrame + 1) % screen.swapchainImageCount;

        if (!ParseVkResult(renderRes))
            WEngine::WLog::ConsoleLog("Something went wrong during rendering!");

        stats.drawCallsLastFrame = stats.drawCallsThisFrame;
        stats.drawCallsThisFrame = 0;
    }
}
#endif