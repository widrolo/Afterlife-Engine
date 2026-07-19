#pragma once

// This file does not contain LLM generated documentation

#include <Engine/Types/Rendering/GPU/Shader.h>
#include <Engine/Types/Nullable.h>
#include <SDL3/SDL.h>

#include <Engine/Types/Rendering/Color.h>
#include <Engine/Types/Rendering/GPU/Framebuffer.h>
#include <Engine/imgui/imgui.h>
#include <Engine/Types/Rendering/InstanceData.h>
#include <Engine/Types/Rendering/LightingInfo.h>
#include <Engine/Types/Rendering/LineInfo.h>
#include <Engine/Types/Rendering/GPU/Line.h>
#include <Engine/Types/Rendering/MeshInfo.h>
#include <Engine/Types/Rendering/ShaderDefinition.h>
#include <Engine/Types/Rendering/ShaderSettings.h>
#include <Engine/Types/Rendering/TextureInfo.h>
#include <Engine/Types/Rendering/GPU/Material.h>
#include <Engine/Types/Rendering/GPU/Model.h>
#include <Engine/Types/Rendering/GPU/StatBufKey.h>
#include <Engine/Types/Rendering/Iris/InstThreadedList.h>
#include <Engine/Types/Rendering/Iris/IrisAssetComms.h>

/**
 * This class is the primary interface to talk to the GPU.
 * The different APIs implement it themselves, and are selected in the engine defines.
 * @note All types used are strictly API independent.
 * @see EngineDefines.h
 */
class Iris
{
public:
    /**
     * If called, the behavior of the editor changes to accommodate the editors needs.
     */
    static void SETTING_EnableEditorMode();

    /**
     * Initializes the GPU API for rendering.
     * @param window Pointer to a windows which will be used to create a Surface.
     * @return True on success.
     */
    static bool SETTING_InitGPUApi(SDL_Window* window);

    /**
     * Sets up things that a given GPU API might need for the duration of the tick frame.
     * @note For example: Vulkan Starts the texture upload buffer.
     */
    static void SETTING_BeginNewPreFrame();

    /**
     * Sets up things that a given GPU API might need for the duration of the draw frame.
     */
    static void SETTING_BeginNewFrame();

    /**
     * Sets the Viewport size/resolution of the actively selected framebuffer.
     * @param size New size of the Viewport.
     */
    static void SETTING_SetViewportSize(WEngine::Vector2 size);

    /**
     * Updates the lighting info on the GPU that is later used by shaders.
     * @param light World lighting information.
     */
    static void SETTING_SetLighting(const WEngine::LightingInfo& light);


    // ----------------------- Shaders -----------------------

    /**
     * Retrieves the definition for a shader by its name.
     * @param shaderName Name of the shader.
     * @return The definition of the shader which can be null.
     * @note The file path and shader name are usually synonymous, please name the shader after the file path.
     */
    static WEngine::Nullable<WEngine::ShaderDefinition> GetShaderDef(const std::string& shaderName);

    /**
     * Retrieves the definition for a shader by quering a material that uses it.
     * @param matQuery The material which uses the shader.
     * @return The definition of the shader which can be null.
     */
    static WEngine::Nullable<WEngine::ShaderDefinition> GetShaderDef(WEngine::Material matQuery);

    /**
     * Retrieves a material handle by its name.
     * @param matName Name of the material.
     * @return A handle to the material which can be null.
     * @note The file path and material name are usually synonymous, please name the material after the file path.
     */
    static WEngine::Nullable<WEngine::Material> GetMaterial(const std::string &matName);

    /**
     * Retrieves a shader handle by its name.
     * @param shaderName Name of the shader.
     * @return A handle to the shader which can be null.
     * @note The file path and shader name are usually synonymous, please name the shader after the file path.
     */
    static WEngine::Nullable<WEngine::Shader> GetShader(const std::string &shaderName);

    /**
     * Retrieves a shader handle by quering a material that uses it.
     * @param matQuery The material which uses the shader.
     * @return A handle to the shader which can be null.
     */
    static WEngine::Nullable<WEngine::Shader> GetShader(WEngine::Material matQuery);

    /**
     * Finds a material in the materials path and compiles it.
     * @param matName File path of the material.
     * @return A handle to the material which can be null.
     * @note The file path and material name are usually synonymous, please name the material after the file path.
     */
    static WEngine::Nullable<WEngine::Material> ALLOC_CompileMaterial(const std::string& matName);

    // ----------------------- Models ------------------------

    /**
     * Retrieves a model handle by its name.
     * @param modelName Name of the model.
     * @return A handle to the model which can be null.
     */
    static WEngine::Nullable<WEngine::Model> GetModel(const std::string& modelName);

    /**
     * Takes the provided mesh data and loads it onto the GPU for rendering later.
     * @param model Mesh data to be loaded onto the GPU.
     * @param addToBVH Whether the model should be included in the BVH. This is synonymous with shadow casting.
     * @return A handle to the model which can be null.
     */
    static WEngine::Nullable<WEngine::Model> ALLOC_CreateModel(const WEngine::MeshInfo& model, bool addToBVH = true);

    // ----------------------- Drawing -----------------------

    /**
     * Clears a framebuffer with a specific color and begins rendering on it.
     * @param clearColor The color that the framebuffer is cleared at.
     * @note This function begins rendering on the selected framebuffer.
     */
    static void DRAWCALL_ClearFrame(WEngine::Color clearColor);

    /**
     * Invokes a drawcall and draws a single model to the screen.
     * @param model A handle to the model to be drawn.
     * @param material A handle to the material and hence the shader that is used for drawing.
     * @param mvp The Model View Projection matrix.
     */
    static void DRAWCALL_DrawModel(WEngine::Model model, WEngine::Material material, const WEngine::Mat4x4& mvp);

    /**
     * Invokes a drawcall and draws a collection of models to the screen.
     * @param model A handle to the model to be drawn.
     * @param material A handle to the material and hence the shader that is used for drawing.
     * @param vp The View Projection matrix.
     * @param instanceMats A collection of model matrices that will be used for instanced drawing.
     * @note Use this function when drawing lots of object that change transform every frame.
     */
    static void DRAWCALL_DrawModelInstanced(WEngine::Model model, WEngine::Material material,
        const WEngine::Mat4x4& vp, const wtl::vector<WEngine::InstanceData>& instanceMats);

    /**
     * Invokes a drawcall and draws a collection of models to the screen. The collection is a preloaded buffer on the GPU.
     * @param sectorKey The sector that invoked the draw.
     * @param model A handle to the model to be drawn.
     * @param material A handle to the material and hence the shader that is used for drawing.
     * @param vp The View Projection matrix.
     * @note Use this function when drawing lots of object that never change transform.
     * @see Iris::AddStationaryObjects
     */
    static void DRAWCALL_DrawModelInstancedStationary(WEngine::StatBufKey sectorKey, WEngine::Model model,
        WEngine::Material material, const WEngine::Mat4x4& vp);

    /**
     * Invokes a draw call and copies over a framebuffer to the selected framebuffer using a shader that may or may not perform post processing on it.
     * @param ppShader A handle to the post processing shader to be used.
     * @param sampleFrameBuffer A handle to the framebuffer that will be sampled for use by the shader.
     */
    static void DRAWCALL_DrawPostProcess(WEngine::Shader ppShader, WEngine::Framebuffer sampleFrameBuffer);

    /**
     * Draws the screen framebuffer to the monitor and finishes up the rendering pass.
     * @param window The window which it will render to.
     * @note This function should be the last time Iris would be getting called for the iteration of the game loop.
     */
    static void DRAWCALL_DrawToDisplay(SDL_Window* window);

    // --------------------- Framebuffers --------------------

    /**
     * Creates a framebuffer.
     * @param resolution The resolution of the framebuffer,
     * @return A handle to the framebuffer which can be null.
     */
    static WEngine::Nullable<WEngine::Framebuffer> ALLOC_CreateFramebuffer(const WEngine::Vector2& resolution);

    /**
     * Selects a framebuffer for rendering.
     * @param framebuffer The framebuffer which will be selected for rendering.
     * @note This does not start rendering, it merely selects it in Iris.
     * @see Iris::DRAWCALL_ClearFrame
     */
    static void SETTING_SelectFramebufferForRender(WEngine::Framebuffer framebuffer);

    /**
     * Selects the screen framebuffer, aka the buffer which is later used to draw to the monitor.
     * @note This does not start rendering, it merely selects it in Iris.
     * @see Iris::DRAWCALL_ClearFrame
     */
    static void SETTING_SelectFramebufferScreenForRender();

    /**
     * Finishes the rendering for a framebuffer.
     * @note This ends rendering and deselects the framebuffer.
     */
    static void SETTING_FinishFramebufferRender();

    /**
     * Prepares a framebuffer to be sampled by a shader.
     * @param framebuffer The framebuffer to be sampled.
     */
    static void SETTING_PrepareFramebufferForSampling(WEngine::Framebuffer framebuffer);

    // ------------------------ Memory -----------------------

    /**
     * Returns the internally counted VRAM usage.
     * @return VRAM usage in bytes.
     */
    static uint64 GetVramUsage();

    /**
     * Returns the internally counted drawcall count.
     * @return Drawcall count.
     * @note ImGui does not get counted.
     */
    static uint32 GetDrawCallCountLastFrame();

    /**
     * Whether the current frame is the first frame in the game loop.
     * @return True is the frame is the first frame.
     */
    static bool IsFirstFrame();

    /**
     * Finds a free stationary buffer that can be used by a game sector to populate with model instances.
     * @return A handle to a stationary buffer key which can be null.
     */
    static WEngine::Nullable<WEngine::StatBufKey> RequestStationaryBufferKey();

    /**
     * Adds a collection of instances to a sector owned stationary buffer.
     * @param key A handle to the key to the sector owned stationary buffer.
     * @param model A handle to the model to be stored.
     * @param material A handle to the material to be stored.
     * @param instanceMats A collection of model instances to be stored for instancing.
     */
    static void AddStationaryObjects(WEngine::StatBufKey key, WEngine::Model model, WEngine::Material material,
        const wtl::vector<WEngine::InstanceData>& instanceMats);

    // --------------------- Ray Tracing ---------------------

    /**
     * Adds a model to the BVH for ray tracing.
     * @param model A handle to the model to be added to the BVH.
     * @param instance Instance data.
     * @note The model must have been created as a BVH model.
     */
    static void SETTING_AddModelInstanceToBVH(WEngine::Model model, const WEngine::InstanceData& instance);

    // ------------------------- ImGui -----------------------

    /**
     * Sets up and configures ImGui.
     * @param window The window which it will render to.
     */
    static void SETTING_ConfigureImGui(SDL_Window* window);

    /**
     * Calls reset on ImGui.
     */
    static void DRAWCALL_ResetImGui();

    /**
     * Draws ImGui.
     */
    static void DRAWCALL_DrawImGui();

    /**
     * Casts a framebuffer into an ImGui Texture.
     * @param framebuffer A handle to the framebuffer.
     * @return An ImGui texture which can be null.
     */
    static WEngine::Nullable<ImTextureID> FramebufferToImGui(WEngine::Framebuffer framebuffer);

    // --------------------- Asset Repo ----------------------

    /**
     * Asset repostory to Iris communication channel.
     * @param mission Communication mission.
     * @warning This function shall only be called by AssetRepo.
     */
    static void AssetIrisCommunication(WEngine::AssetIrisCommunication& mission);
};
