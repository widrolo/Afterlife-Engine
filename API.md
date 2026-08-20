# API Reference

> This file is generated from the public headers of the engine's core systems, handlers, and the asset repo. **Refer to this file instead of reading header/implementation files.** It is kept in sync by the maintainer; update it whenever the public API changes.

Covers everything in `src/Engine/Core/System/` and `src/Engine/Core/Handlers/`.

- Opaque handles (e.g. `BufferHandle`) are created via the API and returned to the caller; the caller never inspects their internals.
- `sizeT` is always 64 bit (see Code Conventions in AGENTS.md).

---

## System Abstractions

All-static facade classes. See AGENTS.md for the philosophy.

### Iris (`src/Engine/Core/System/Iris.h`)

Namespace `Iris`. The low-level, explicitly recorded command API for rendering. The caller creates opaque handles, records commands into per-frame command buffers, and submits them. Vulkan-only at the moment.

#### Lifecycle & Boot

| Function | Description |
| --- | --- |
| `bool Init(const InitDesc& desc)` | Initializes the rendering backend. Must be called once at startup. |
| `void Shutdown()` | Tears down the renderer and frees all GPU resources. |
| `uint32 GetCurrentFrameIndex()` | Index of the current in-flight frame. |
| `uint32 GetFramesInFlight()` | Number of frames in flight, as configured in `InitDesc`. |
| `bool IsFirstFrame()` | True on the very first rendered frame. |
| `void BeginFrame()` | Marks the start of a frame. Call once per frame before recording commands. |
| `void EndFrame()` | Marks the end of the frame's command recording. |
| `void ConfigureImGui()` | Integrates ImGui with the renderer (pipeline, fonts, etc). |
| `void ImGuiNewFrame()` | Starts a new ImGui frame. Call before recording ImGui draw data. |
| `void ImGuiRenderDrawData(CommandBufferHandle cmd)` | Records ImGui's generated draw data into the given command buffer. |
| `WEngine::Nullable<ImTextureID> TextureToImGui(TextureHandle texture)` | Converts a texture handle to an ImGui texture ID, or empty if unsupported. |

#### Resource Creation

| Function | Description |
| --- | --- |
| `BufferHandle CreateBuffer(const BufferDesc& desc)` | Creates a GPU buffer. |
| `BufferHandle CreateBuffer(const BufferDesc& desc, const byte* initialData, sizeT initialDataSize)` | Creates a GPU buffer and fills it with the given data immediately. |
| `TextureHandle CreateTexture(const TextureDesc& desc)` | Creates a GPU texture. |
| `SamplerHandle CreateSampler(const SamplerDesc& desc)` | Creates a texture sampler. |
| `ShaderHandle CreateShader(const ShaderStageDesc& desc)` | Creates a shader module from SPIR-V bytecode. |
| `ResourceTableLayoutHandle CreateResourceTableLayout(const ResourceTableLayoutDesc& desc)` | Creates a resource table layout describing the bindings of a table. |
| `ResourceTableHandle CreateResourceTable(ResourceTableLayoutHandle layout)` | Creates a resource table (descriptor set) from a layout. |
| `GraphicsPipelineHandle CreateGraphicsPipeline(const GraphicsPipelineDesc& desc)` | Creates a graphics pipeline. |
| `ComputePipelineHandle CreateComputePipeline(const ComputePipelineDesc& desc)` | Creates a compute pipeline. |
| `FramebufferHandle CreateFramebuffer(const FramebufferDesc& desc)` | Creates a framebuffer from color/depth attachments. |

#### Resource Updates & Destruction

| Function | Description |
| --- | --- |
| `void UpdateBuffer(BufferHandle buffer, sizeT dstOffset, const byte* data, sizeT size)` | Uploads data into a buffer starting at the given offset. |
| `void UpdateResourceTable(ResourceTableHandle table, const ResourceTableUpdateDesc& update)` | Binds buffers/textures/samplers to the given resource table. |
| `void DestroyBuffer(BufferHandle buffer)` | Destroys a GPU buffer. |

#### Swapchain

| Function | Description |
| --- | --- |
| `TextureHandle AcquireSwapchainTexture()` | Gets the current swapchain image as a texture for rendering into. |
| `FramebufferHandle GetSwapchainFramebuffer()` | Gets the swapchain framebuffer to use as the render target. |
| `void Present()` | Presents the rendered swapchain image to the screen. |

#### Command Buffers

| Function | Description |
| --- | --- |
| `CommandBufferHandle CreateCommandBuffer(QueueType queue)` | Creates a command buffer on the given queue (defaults to Graphics). |
| `CopyBufferHandle CreateCopyBuffer()` | Creates a copy command buffer for async transfers. |
| `void BeginCommandBuffer(CommandBufferHandle cmd)` | Starts recording commands into the buffer. |
| `void EndCommandBuffer(CommandBufferHandle cmd)` | Stops recording. |
| `void SubmitCommandBuffer(CommandBufferHandle cmd)` | Submits the recorded commands to the GPU. |

#### Passes

| Function | Description |
| --- | --- |
| `void BeginRenderPass(CommandBufferHandle cmd, const RenderPassBeginDesc& desc)` | Starts a render pass (dynamic rendering) against a framebuffer. |
| `void EndRenderPass(CommandBufferHandle cmd)` | Ends the current render pass. |
| `void BeginComputePass(CommandBufferHandle cmd)` | Starts a compute pass. |
| `void EndComputePass(CommandBufferHandle cmd)` | Ends the current compute pass. |
| `void BeginCopyPass(CopyBufferHandle cmd)` | Starts an async copy pass. |
| `void EndCopyPass(CopyBufferHandle cmd)` | Ends the current copy pass. |
| `bool IsCopyPassDone(CopyBufferHandle cmd)` | Whether the copy pass has finished executing. |

#### Resource Binding

| Function | Description |
| --- | --- |
| `void BindGraphicsPipeline(CommandBufferHandle cmd, GraphicsPipelineHandle pipeline)` | Binds a graphics pipeline. |
| `void BindComputePipeline(CommandBufferHandle cmd, ComputePipelineHandle pipeline)` | Binds a compute pipeline. |
| `void BindResourceTable(CommandBufferHandle cmd, GraphicsPipelineHandle pipeline, uint32 slot, ResourceTableHandle table)` | Binds a resource table to the given pipeline slot. |
| `void SetPushConstants(CommandBufferHandle cmd, GraphicsPipelineHandle pipeline, const byte* data, sizeT size)` | Pushes constant data to the pipeline. |
| `void BindVertexBuffers(CommandBufferHandle cmd, uint32 firstBinding, const wtl::vector<BufferHandle>& buffers, const wtl::vector<sizeT>& offsets)` | Binds vertex buffers, each with an offset. |
| `void BindIndexBuffer(CommandBufferHandle cmd, BufferHandle buffer, sizeT offset)` | Binds an index buffer. |
| `void SetViewport(CommandBufferHandle cmd, const Viewport& viewport)` | Sets the viewport transform. |

#### Draw Calls

| Function | Description |
| --- | --- |
| `void Draw(CommandBufferHandle cmd, sizeT vertexCount, sizeT instanceCount, sizeT firstVertex, sizeT firstInstance)` | Non-indexed draw. |
| `void DrawIndexed(CommandBufferHandle cmd, sizeT indexCount, sizeT instanceCount, sizeT firstIndex, int32 vertexOffset, sizeT firstInstance)` | Indexed draw. |
| `void DrawIndirect(CommandBufferHandle cmd, BufferHandle argBuffer, sizeT offset, sizeT drawCount, sizeT stride)` | Non-indexed indirect draw; arguments live in the buffer. |
| `void DrawIndexedIndirect(CommandBufferHandle cmd, BufferHandle argBuffer, sizeT offset, sizeT drawCount, sizeT stride)` | Indexed indirect draw. |

#### Dispatches

| Function | Description |
| --- | --- |
| `void Dispatch(CommandBufferHandle cmd, sizeT groupCountX, sizeT groupCountY, sizeT groupCountZ)` | Launches a compute workload. |
| `void DispatchIndirect(CommandBufferHandle cmd, BufferHandle argBuffer, sizeT offset)` | Compute dispatch whose arguments live in the buffer. |

#### Copies

| Function | Description |
| --- | --- |
| `void CopyBufferToBuffer(CommandBufferHandle cmd, BufferHandle dst, sizeT dstOffset, BufferHandle src, sizeT srcOffset, sizeT size)` | Copies data between buffers. |
| `void CopyBufferToTexture(CopyBufferHandle cmd, BufferHandle src, sizeT srcOffset, TextureHandle dst)` | Copies a buffer (typically a staging buffer) into a texture. |

#### Stats

| Function | Description |
| --- | --- |
| `GPUInfo GetGPUInfo()` | GPU name and total VRAM. |
| `VramStats GetVRAMStats()` | VRAM usage broken down by resource type. |
| `DrawStats GetDrawCallStats()` | Draw/dispatch/copy call counters. |
| `BindStats GetBindingStats()` | Pipeline/buffer/table bind counters. |

### Haptic (`src/Engine/Core/System/Haptic.h`)

Class `Haptic`. All static. Input and haptic output; behaves like Steam Input. See AGENTS.md for the full signal-processing pipeline.

| Function | Description |
| --- | --- |
| `void Init(SDL_Window* window)` | Initializes the input backend with the SDL window. |
| `void FetchInput()` | Polls raw device state into the double-buffered current/previous slots. Call once per frame. |
| `void PollEvents()` | Processes window/device events (controller hotplug, vendor detection, etc.). |
| `void EnableEditorMode()` | Switches Haptic behavior to suit the editor. |
| `void LoadInputMap(const wtl::vector<WEngine::InputSense>& mapContent, const std::string& mapName)` | Loads a parsed input map under the given name. |
| `void SelectInputMap(const std::string& mapName)` | Activates a loaded input map. |
| `void LoadOutputMap(const std::string& mapName)` | Loads an output (rumble/LED) map. |
| `void SelectOutputMap(const std::string& mapName)` | Activates a loaded output map. |
| `void ApplyFullPatch(const wtl::vector<WEngine::InputPatch>& patches)` | Applies a full set of input remapping patches. |
| `void ApplySinglePatch(const WEngine::InputPatch& patch)` | Applies a single input remapping patch. |
| `wtl::vector<WEngine::InputPatch> GetPatchList()` | Returns the currently applied patches. |
| `WEngine::Nullable<bool> GetActionJustPressed(const std::string& actionName)` | Whether the action was pressed this frame. Empty if the sense is missing. |
| `WEngine::Nullable<bool> GetActionHeld(const std::string& actionName)` | Whether the action is currently held. |
| `WEngine::Nullable<bool> GetActionJustReleased(const std::string& actionName)` | Whether the action was released this frame. |
| `WEngine::Nullable<float32> GetFloat(const std::string& floatName)` | Normalized [-1, 1] float sense (triggers, deltas, etc). |
| `WEngine::Nullable<WEngine::Vector2> GetVector(const std::string& vectorName)` | 2D vector sense (joysticks with deadzone, key clusters, etc). |
| `WEngine::Nullable<WEngine::InputVendor> GetVendor()` | Detected controller vendor. |
| `bool GetDebugKeyJustPressed(uint8 keyNum)` | Debug key (F-key) just pressed. Reserved for debugging/tools. |
| `bool GetDebugKeyHeld(uint8 keyNum)` | Debug key held. |
| `bool GetDebugKeyJustReleased(uint8 keyNum)` | Debug key just released. |
| `void Rumble(const std::string& outputName)` | Triggers the rumble output. |
| `void SetLED(const std::string& outputName)` | Sets the controller LED output. |

### OS (`src/Engine/Core/System/OS.h`)

Class `OS`. All static. OS-dependent operations; all types are OS-independent.

| Function | Description |
| --- | --- |
| `std::string GetProcessPath()` | Full path of the current process. |
| `void SetConsoleColor(unsigned char color)` | Sets the console text color. |
| `void CreateNewProcess(const std::string& executable, const wtl::vector<std::string>& arguments)` | Spawns a new process with the given arguments. |
| `wtl::vector<std::string> GetAllFileNamesInDir(const std::string& dir)` | All file names in a directory (non-recursive, full paths). |

### WAllocator (`src/Engine/Core/System/Memory.h`)

Class `WAllocator`. All static. Custom memory interface that tracks total engine memory usage; wraps the malloc allocator internally.

| Function | Description |
| --- | --- |
| `void BootAllocator()` | Starts the allocator. Must only be called at startup. |
| `void ReportExternalAllocation(uint64 size)` | Adds untracked memory to usage, for libraries that don't support a custom allocator. |
| `void ReportExternalFree(uint64 size)` | Removes untracked memory from usage. |
| `void* Allocate(uint64 size)` | Allocates and tracks. |
| `void* AllocateAligned(uint64 size, uint64 alignment)` | Allocates with alignment and tracks. |
| `void* Reallocate(void* ptr, uint64 size)` | Reallocates and tracks. |
| `void* ReallocateAligned(void* ptr, uint64 size, uint64 alignment)` | Reallocates with alignment and tracks. |
| `void Free(void* ptr)` | Frees and tracks. |
| `template<class T, typename... Args> T* Construct(Args&&... args)` | Constructs an object in place and tracks its size. |
| `template<class T> void Destruct(T* p)` | Destroys an object and removes it from tracked usage. |
| `uint64 GetMemoryUsage()` | Current tracked memory usage in bytes. |

Macros: `wNew(size)`, `wNewArr(type, count)`, `wFree(ptr)` are thin wrappers around `Allocate`/`Free`.

### ThreadScheduler (`src/Engine/Core/System/ThreadScheduler.h`)

Class `WEngine::ThreadScheduler`. One scheduler per worker thread; jobs are queued and run on the thread.

| Function | Description |
| --- | --- |
| `ThreadScheduler()` / `~ThreadScheduler()` | Creates/tears down the scheduler and its thread. |
| `void AddJob(const Job& job)` | Queues a job for this thread. A `Job` is a `std::function<void*(void*)>`. |
| `void RunJobs()` | Runs all queued jobs on this thread. |

### GPUSettings (`src/Engine/Core/System/GPUSettings.h`)

Global renderer configuration structs, edited via the editor UI (`_GLOBAL_CEX_`). Load once at boot.

- `InvalidResultAction`: what happens on an invalid handle/result: `LetGo`, `Stall`, or `Abort`.

`GPUSettings`:
| Field | Default | Description |
| --- | --- | --- |
| `stationaryInstBufferSize` | `64 * KB` | Size of each stationary instance buffer. |
| `maxStationaryInstBuffers` | `512` | Maximum number of stationary instance buffers. |
| `invalidHandleAction` | `Abort` | Behavior when an invalid handle is used. |

`GPUSettingsVulkan` (Vulkan-specific overrides):
| Field | Default | Description |
| --- | --- | --- |
| `useWAllocator` | `true` | Route GPU allocations through `WAllocator` (VMA). |
| `enableValidation` | debug-only | Enable the Vulkan validation layers. |
| `invalidResultAction` | `LetGo` | Behavior on an invalid Vulkan result. |
| `validationErrorAction` | `Abort` | Behavior on a validation-layer error. |
| `maxInstanceBufferSize` | `4` | Maximum instance buffer size. |
| `maxMaterialCount` | `512` | Maximum number of materials. |
| `maxPPShaders` | `512` | Maximum number of post-processing shaders. |

---

## Deep Core Handlers

Singletons stored globally in `CoreSystems`. They primarily interact with System Abstractions.

### Render Handler (`src/Engine/Core/Handlers/RenderHandler.h`)

Class `WEngine::RenderHandler`. Middle man between components that wish to draw and Iris. Records render missions during the draw part of the game loop, optimizes them, and hands them to Iris.

| Function | Description |
| --- | --- |
| `RenderHandler()` | Constructor. |
| `void EnableEditorMode(const Vector2& viewportResolution)` | Changes behavior to account for the editor (ATK), using the given viewport resolution. |
| `Framebuffer EditorGetViewportFramebuffer()` | The framebuffer meant for the editor viewport. Only called by ATK. |
| `void BeginFrame()` | Starts the render pass and begins collecting render missions. Game loop only. |
| `void RenderFrame()` | Finishes mission collection, optimizes, and renders everything. Game loop only. |
| `void RegisterCamera(CameraComponent* camera)` | Sets the camera used to render the frame. |
| `void AddToRenderQueue(RenderMission& mission)` | Records a render mission for the frame. |
| `void RegisterTexture(Iris::TextureHandle handle)` | Registers a texture handle so it can be bound during rendering. |

### Audio Handler (`src/Engine/Core/Handlers/AudioHandler.h`)

Class `WEngine::AudioHandler`. Manages creation, swapping, and playing of audio clips via audio players. To be reworked when Echo arrives.

| Function | Description |
| --- | --- |
| `void AudioTick()` | Does nothing for now. |
| `AudioPlayer* NewAudioPlayer(AudioClip* clip)` | Creates a new audio player for the clip; returns nullptr if the clip is invalid. |
| `void SwapAudioClip(AudioPlayer* player, AudioClip* newClip)` | Swaps the clip played by an existing player. |
| `void PlayAudioPlayer(AudioPlayer* player)` | Plays the player (no-op if already playing or nullptr). |
| `void PauseAudioPlayer(AudioPlayer* player)` | Pauses the player (no-op if already paused or nullptr). |

### Input (`src/Engine/Core/Handlers/Input.h`)

Class `Input`. All static. Not a handler itself, but the interface gameplay components use to query Haptic. Also parses YAML input maps into `InputSense` entries and loads them into Haptic.

| Function | Description |
| --- | --- |
| `void LoadInputMap()` | Parses the YAML input maps and loads them into Haptic. |
| `bool GetAction(const std::string& name, PressType press)` | Queries an action sense with the given press type (`Press`, `Hold`, `Release`). |
| `float32 GetFloat(const std::string& name)` | Queries a float sense. |
| `WEngine::Vector2 GetVector(const std::string& name)` | Queries a vector sense. |

---

## High Level Core Handlers

Also stored globally in `CoreSystems`, but closer to gameplay. May touch System Abstractions but do not rely on them.

### Widget Handler (`src/Engine/Core/Handlers/WidgetHandler.h`)

Class `WEngine::WidgetHandler`. Handles the creation, drawing, and removal of engine UI components (Widgets) via ImGui.

| Function | Description |
| --- | --- |
| `void InitSystemWidgets()` | Initializes the built-in system widgets with their functionality. |
| `void DrawWidgets()` | Draws all enabled system and game widgets. |
| `void AddGameWidget(const std::shared_ptr<Widget>& widget)` | Adds a game widget to be handled and rendered. |
| `void RemoveGameWidget(const std::shared_ptr<Widget>& widget)` | Removes a game widget from being handled and rendered. |

`SysWidgetTypes` enumerates the built-in system widget slots (System, EngineControl, GameSystem, Statistics, Timings, Peripherals, SectorWatch, PhysicsWatch, DebugFlags, RenderWatch, TimeWatch).

### RNG Handler (`src/Engine/Core/Handlers/RNGHandler.h`)

Class `WEngine::RNGHandler`. Random number generation. Soon to be deprecated.

| Function | Description |
| --- | --- |
| `int64 GetRandomInt(int64 min, int64 max)` | Random integer in `[min, max]`, inclusive. |
| `float32 GetRandomFloat(float32 min, float32 max)` | Random float in `[min, max]`, inclusive. |
| `Vector2 GetRandomVector2(float32 maxLength)` | Random 2D vector with length in `[0, maxLength]`. |

### Physics Handler (`src/Engine/Core/Handlers/PhysicsHandler.h`)

Class `WEngine::PhysicsHandler`. Simulates physics using Box3D.

| Function | Description |
| --- | --- |
| `void Tick()` | Advances the physics simulation one step. |
| `PhysicsBodyHandle CreateBody(PhysicsBodyType type, Entity* entity)` | Creates a physics body of the given type tied to an entity. |
| `void ChangeBodyPosition(PhysicsBodyHandle body, const Vector3& position)` | Moves a body to the given position. |
| `void ChangeBodyRotation(PhysicsBodyHandle body, const Quaternion& rotation)` | Rotates a body to the given rotation. |
| `void AttachBox(PhysicsBodyHandle body, const Vector3& size, const Vector3& offset)` | Attaches a box collider to the body. |
| `void AttachMesh(PhysicsBodyHandle body, const MeshInfo& mesh)` | Attaches a mesh collider to the body. |

### Time Handler (`src/Engine/Core/Handlers/TimeHandler.h`)

Class `WEngine::TimeHandler`. Manages in-game time and date, used for lighting.

| Function | Description |
| --- | --- |
| `void Update(float32 dt)` | Advances the in-game clock by the delta time. |
| `void SetDate(const Date& date)` | Sets the in-game date. |
| `void SetTime(const Time& time)` | Sets the in-game time. |
| `Date GetDate() const` | Current in-game date. |
| `Time GetTime() const` | Current in-game time. |

### Job Handler (`src/Engine/Core/Handlers/JobHandler.h`)

Class `WEngine::JobHandler`. Manages the worker threads and their schedulers for parallel job execution.

| Function | Description |
| --- | --- |
| `ThreadScheduler* GetThreadScheduler(uint8 threadID)` | Returns the scheduler for the given thread. |
| `ThreadScheduler* GetThreadScheduler(uint8 threadID, uint64 ownershipProof)` | Returns the scheduler for the given thread, proving ownership. |
| `void AddScheduledJob(const Job& jobFunction, JobLengthEstimation estimation)` | Schedules a job with an estimated length (`VeryShort` to `VeryLong`) for load balancing. |

---

## Asset Repo

### AssetRepo (`src/Engine/Core/Handlers/AssetRepo.h`)

Class `WEngine::AssetRepo`. Primary location for getting all kinds of files. Handles loading and unloading of game assets: sprites, shaders, YAML files, atlas info, audio clips, UI sheets, meshes, and textures. Per engine constraints, everything is loaded at boot.

| Function | Description |
| --- | --- |
| `void LoadAllGPUAssets()` | Loads all graphical assets up front. Can only be called once. |
| `void TickTextureUpload()` | Uploads textures to VRAM in chunks over time. Call at the beginning of every frame; turns itself off when done. |
| `template<class T> void GetAsset(T& mission)` | Fills out the given asset mission (e.g. `ShaderAssetMission`, `SpirVAssetMission`, `YamlAssetMission`, `AudioClipAssetMission`, `UISheetAssetMission`, `MeshAssetMission`) by name. Return type is void; results go into the mission. |
| `std::string GetDataPath() const` | The data path where assets are stored. |
| `const wtl::vector<AssetRef>& GetAllAssetsInDir(const std::string& dirName)` | All assets in a directory (empty if the directory is empty or doesn't exist). |
| `wtl::vector<AssetRef> GetAllAssetsInDirOfType(const std::string& dirName, AssetType type)` | All assets of a given type in a directory. |
| `uint64 GetFirstAssetInDirOfType(const std::string& dirName, AssetType type)` | UID of the first asset of a type in a directory; 0 if not found. |
| `uint64 GetAssetInDirByName(const std::string& dirName, const std::string& assetName)` | UID of the first asset with the given name (sub name, not project-file name) in a directory; 0 if not found. |
