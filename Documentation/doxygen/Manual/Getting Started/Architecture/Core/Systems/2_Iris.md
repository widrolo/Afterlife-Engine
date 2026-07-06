@page iris Architecture/Core/Systems

# Iris

Iris is by far the most important and most complex core system in Widrolo Engine. It is an abstraction layer for any kind of arbitrary GPU API. While it currently supports only Vulkan, it can be extended trivially by using any kind of current gen GPU API. This is because Iris exposes very high level functionality to the engine which in theory, can be implemented by any current gen API that supports the following features:

- 3D rendering
- Custom shaders
- Instanced rendering (technically optional)
- Drawing to a custom framebuffer
- Ray Tracing

If a GPU API satisfies all these traits, then it can be implemented for Iris. Of course, OpenGL, DX11, Metal 1, GNM do not support ray tracing, which means that they are not supported by Widrolo Engine. However, the aforementioned APIs are also not current gen as stated before.

The way to extend Iris is mentioned in the Systems section of this manual (which coincidentally uses Iris as an example), so I won't be repeating myself.

As mentioned before, Iris provides a high level feature set to interact with the GPU. Using Iris for rendering feels more like Raylib, rather than OpenGL for example. Here is some pseudo code to show how a basic renderer might look like:

``` C++
int main()
{
    Iris::SETTING_InitGPUApi();
    Model cube = Iris::ALLOC_CreateModel(cubeInfo);
    Material mat = Iris::ALLOC_CompileMaterial("cubeMat");

    while (gameRunning)
    {
        Iris::SETTING_BeginNewPreFrame();

        // game logic

        Iris::SETTING_BeginNewFrame();
        Iris::SETTING_SelectFramebufferScreenForRender();
        Iris::SETTING_SetViewportSize(resolution);
        Iris::DRAWCALL_ClearFrame(Color::Black);

        Iris::SETTING_SetLighting(worldLighting);
        Iris::DRAWCALL_DrawModel(cube, mat, cubeMVP);

        Iris::SETTING_FinishFramebufferRender();
        Iris::DRAWCALL_DrawToDisplay(window);
    }
}
``` 

Now that you see exactly how basic it is, let's get a quick overview of the typically relevant functionality Iris provides for building systems:

*Note that I will be shortening the function definition for all functions and not all functions are present; refer to the class reference for an exact overview and deeper explanation. This is just an overview*

#### Shaders & Materials

Shaders and materials by themselves are not going to be discussed here, we will only focus on the way that Iris deals with them. Here are all the functions that take care of shaders and materials:

``` C++
ShaderDefinition    GetShaderDef(string shaderName);
Material            GetMaterial(string matName);
Shader              GetShader(string shaderName);
Shader              GetShader(Material matQuery);
Material            ALLOC_CompileMaterial(string matName);
```

In general, Iris will automatically precompile all shaders at startup, so the shader compilation function is not exposed. Materials, however, are loaded during gameplay. You can always get all kinds of handles which are important for rendering.

#### Models

Here is all the functionality that Iris provides to manage in game models:

``` C++
Model               GetModel(string modelName);
Model               ALLOC_CreateModel(ModelInfo model, bool addToBVH = true);
```

This is trivial, you can load a model into memory and then later retrieve the handle.

If addToBVH is true, then the model is always available for shadow casting.

#### Framebuffers

Framebuffers are a big part of post-processing or similar. Here is what Iris provides:

``` C++
Framebuffer         ALLOC_CreateFramebuffer(Vector2 resolution);
void                SETTING_SelectFramebufferForRender(Framebuffer framebuffer);
void                SETTING_SelectFramebufferScreenForRender();
void                SETTING_FinishFramebufferRender();
void                SETTING_PrepareFramebufferForSampling(Framebuffer framebuffer);
```

Generally, we distinguish between two kinds of framebuffers:
- Custom framebuffer
- Screen framebuffer

A custom framebuffer is a framebuffer you can draw to and then later sample in a shader. The screen framebuffer is the final framebuffer that will later be sent to the monitor.

Note that selecting a framebuffer does not start rendering; that's handled in the next section. However, SETTING_FinishFramebufferRender does end rendering for the given framebuffer. Also, before you pass in any framebuffer into a material, prepare it for sampling first.

#### Drawing

Drawing has a large coverage of functions for all kinds of optimizations:

``` C++
void                DRAWCALL_ClearFrame(WEngine::Color clearColor);
void                DRAWCALL_DrawModel(Model model, Material material, Mat4x4 mvp);
void                DRAWCALL_DrawModelInstanced(Model model, Material material, Mat4x4 vp, vector<InstanceData> instanceMats);
void                DRAWCALL_DrawModelInstancedStationary(StatBufKey sectorKey, Model model, Material material, Mat4x4 vp);
void                DRAWCALL_DrawPostProcess(Shader ppShader, Framebuffer sampleFrameBuffer);
void                DRAWCALL_DrawToDisplay(SDL_Window* window);
```

To render, a framebuffer must be selected. To actually begin the rendering, you must call DRAWCALL_ClearFrame as it calls any relevant functions that will start the rendering process.

Afterward, you can call one of the three functions that renders models. They all behave differently towards instancing.

**DRAWCALL_DrawModel** simply draws a model to the framebuffer using a material.

**DRAWCALL_DrawModelInstanced** instances a collection of models that can move throughout a frame.

**DRAWCALL_DrawModelInstancedStationary** draws a collection of models whose instance matrices are stored statically on the GPU. These are loaded in advance and not per frame, which means that anything drawn with this will never move.

The matrices loaded in advance are placed in a stationary buffer, which is the topic of the next section.

#### Stationary Buffer

The stationary buffer is the highest tier of rendering optimization that Iris has to offer. It's recommended to have as much world geometry be stationary.

``` C++
StatBufKey RequestStationaryBufferKey();
void AddStationaryObjects(StatBufKey key, Model model, Material material, vector<InstanceData> instanceMats);
```

We get a new type here: the stationary buffer key. That is because in Iris, each sector of the world has its own key to a stationary buffer. This is due to the nature of the buffer: unloading a sector would mean that we would have to rewrite the whole buffer for the whole world. So, instead, every sector gets its own preallocated buffer (512 in total).

It enables a whole new kind of optimization: a sector should pack as much of one item as possible. However, that makes it impossible to perform culling on stationary objects, the buffer is immutable per frame.

The previously mentioned **DRAWCALL_DrawModelInstanced** function does not have this issue. Those models can be culled and are exempt from the limitations of the stationary buffer.

