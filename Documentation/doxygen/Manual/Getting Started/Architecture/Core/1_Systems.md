@page systems Architecture/Core

# Systems Abstractions

The core idea of the Systems Abstractions layer is basically to ensure that the entire engine and subsequently the game looks and works identically across all platforms.

The method we use is to create a class with static members and no state. The class shall not take or return arguments from a system-specific class. For example, Iris will never return a VkImage object, but rather a handle that internally points to a VkImage.

The implementation is defined in its own translation units with an ifdef guard that enables the translation units if the system is selected for compilation, and disables if another system is selected.

Taking a look at a simplified Iris for a quick example, the base class would look like this:

``` C++
// Engine/Core/Systems/Iris.h

class Iris
{
public:
    static bool SETTING_InitGPUApi(SDL_Window* window);
    static WEngine::Nullable<WEngine::Material> ALLOC_CompileMaterial(const std::string& matName);
    static void DRAWCALL_DrawModel(WEngine::Model model, WEngine::Material material, const WEngine::Mat4x4& mvp);
}

```

Then in the simplified implementation:

``` C++
// Engine/Core/Systems/IrisVulkan.cpp

#include <Engine/EngineDefines.h>
#if GPU_BACKEND == GPU_VULKAN
#include <vulkan/vulkan.h>

VulkanContext ctx;

bool Iris::SETTING_InitGPUApi(SDL_Window *window)
{
    if (!SetupVkInstance(ctx))
        return false;
}

WEngine::Nullable<WEngine::Material> Iris::ALLOC_CompileMaterial(const std::string& matName)
{
    WEngine::Material matHandle = CompileMaterial(ctx, matName);

    if (matHandle == 0)
        return WEngine::Nullable<WEngine::Material>();

    return WEngine::Nullable<WEngine::Material>(matHandle);
}

void Iris::DRAWCALL_DrawModel(WEngine::Model model, WEngine::Material material, const WEngine::Mat4x4& mvp)
{
    Vulkan_Material vkMat = ctx.loadedMaterials[material - 1];
    Vulkan_Shader vkShader = ctx.loadedShaders[vkMat.materialShaderHandle - 1];

    if (!vkMat.hasTextures)
        vkCmdBindDescriptorSets(GetFbCmdBuff(ctx), VK_PIPELINE_BIND_POINT_GRAPHICS, vkShader.pipelineLayout, 0, 1, &vkMat.materialDescriptorSet, 0, nullptr);
    vkCmdDrawIndexed(GetFbCmdBuff(ctx), vkModel.indexCount, 1, 0, 0, 0);
}

#endif

``` 

Note that all the Vulkan related state is contained within VulkanContext, which itself is a global variable contained in the translation unit.
