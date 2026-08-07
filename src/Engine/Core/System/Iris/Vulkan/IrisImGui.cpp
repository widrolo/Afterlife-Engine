#if GPU_BACKEND == GPU_VULKAN

#include <Engine/Core/System/Iris.h>

#include "IrisGlobals.h"
#include <SDL3/SDL.h>
#include "Engine/imgui/backends/imgui_impl_sdl3.h"
#include "Engine/imgui/backends/imgui_impl_vulkan.h"
#include "Engine/Util/Log.h"
#include "Helpers/Helpers.h"
#include "Helpers/ImGui.h"

namespace Iris
{
    void ConfigureImGui()
    {
        static VkFormat swapFormat = FindBestSwapchainFormat();

        VkPipelineRenderingCreateInfo pipeInfo{};
        pipeInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        pipeInfo.colorAttachmentCount = 1;
        pipeInfo.pColorAttachmentFormats = &swapFormat;
        pipeInfo.depthAttachmentFormat = FindBestDepthFormat();


        ImGui_ImplSDL3_InitForVulkan((SDL_Window*)screen.window);
        ImGui_ImplVulkan_InitInfo initInfo{};
        initInfo.ApiVersion = GetVulkanVersion();
        initInfo.Instance = vcore.instance;
        initInfo.PhysicalDevice = vcore.gpuPhysicalDevice;
        initInfo.Device = vcore.gpuDevice;
        initInfo.Allocator = vcore.allocator;
        initInfo.QueueFamily = queues.primaryDrawQueueFamilyIndex;
        initInfo.Queue = queues.primaryDrawQueue;
        initInfo.ImageCount = screen.swapchainImageCount;
        initInfo.MinImageCount = screen.swapchainImageCount;
        initInfo.DescriptorPoolSize = 8;
        initInfo.UseDynamicRendering = true;
        initInfo.PipelineInfoMain.PipelineRenderingCreateInfo = pipeInfo;
        initInfo.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        ImGui_ImplVulkan_Init(&initInfo);
    }

    void ImGuiNewFrame()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiRenderDrawData(CommandBufferHandle cmd)
    {
        if (cmd == 0 || cmd > loadedCommandBuffers.size())
        {
            WEngine::WLog::SetConsoleError();
            WEngine::WLog::ConsoleLog("Invalid command buffer handle, refusing to draw ImGui!");
            return;
        }
        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), GetCurrentCmdBuff(cmd));
    }

    WEngine::Nullable<ImTextureID> TextureToImGui(TextureHandle texture)
    {
        if (texture == 0 || texture > loadedTextures.size())
        {
            WEngine::WLog::SetConsoleError();
            WEngine::WLog::ConsoleLog("Invalid texture handle, refusing to create ImGui texture!");
            return {};
        }

        Vulkan_Texture& tex = loadedTextures[texture - 1];

        if (tex.imGuiDescriptorSet != VK_NULL_HANDLE)
            return (ImTextureID)tex.imGuiDescriptorSet;

        VkDescriptorSet descriptorSet = ImGui_ImplVulkan_AddTexture(imGuiSampler, tex.imageView,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        if (descriptorSet == VK_NULL_HANDLE)
        {
            WEngine::WLog::SetConsoleError();
            WEngine::WLog::ConsoleLog(std::format("Failed to create ImGui texture from {}!", tex.debugName));
            return {};
        }

        tex.imGuiDescriptorSet = descriptorSet;

        return (ImTextureID)descriptorSet;
    }
}


#endif