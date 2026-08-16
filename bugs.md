# Bugs

Review findings for the Iris refactor and surrounding systems. Status keys: 🔴 critical / 🩷 performance / 🟡 logic & API / 🟢 conventions. The two compile-blocking issues (stale CMake source list, deleted `VulkanCore.h` include) have been fixed and are not listed.

---

## 🔴 Critical (crashes, UB, corruption, incorrect rendering/gameplay)

### 1. Render target vs acquired swapchain image mismatch
- **Location:** `src/Engine/Core/System/Iris/Vulkan/IrisRecording.cpp:170,196,238` / `IrisSwapchain.cpp:23,43`
- **Issue:** `BeginRenderPass`/`EndRenderPass` transition, clear and render `displayTarget.targetImages[screen.currentFrame]`, while `Present()` presents `targetImages[swapchainCurrentImage]` — the index actually returned by `vkAcquireNextImageKHR`. `currentFrame` is an independent per-frame counter. The two only coincide while the swapchain returns images in strict round-robin order; with `VK_PRESENT_MODE_MAILBOX_KHR` (explicitly preferred in `Helpers/Swapchain.cpp:21`) or any dropped/slow frame they desync → you present an image that was never transitioned to `PRESENT_SRC` and render to one that was never acquired. Likely culprit for the "black/stale frame" behavior.
- **Fix:** Render/transition to `targetImages[swapchainCurrentImage]` (the acquired index), keep `currentFrame` for the semaphore/fence arrays.

### 2. Hardcoded 1920x1080 render area
- **Location:** `src/Engine/Core/System/Iris/Vulkan/IrisRecording.cpp:213`, `src/Engine/Core/Handlers/RenderHandler.cpp:105`
- **Issue:** `renderArea.extent = {1920, 1080}` and the viewport are hardcoded, but the swapchain images use `capabilities.currentExtent` and the depth image uses `EngineSettings::resolution` (`Helpers/Swapchain.cpp:99`). On any display that isn't 1080p the render area exceeds the attachments → validation errors and a partially written depth buffer.
- **Fix:** Derive the render area and viewport from the actual swapchain extent.

### 3. OOB read of the SDL extension list on non-Windows
- **Location:** `src/Engine/Core/System/Iris/Vulkan/Helpers/Helpers.cpp:513-515`
- **Issue:**
  ```cpp
  int count_extensions = count_instance_extensions + 1;
  for (int i = 1; i < count_extensions; i++)
      extensions.emplace_back(instance_extensions[i]);
  ```
  Reads `instance_extensions[count]` (one past the end) and silently skips extension index 0 (potentially the surface extension).
- **Fix:** `for (int i = 0; i < count_instance_extensions; i++)`.

### 4. Allocator free mismatches / double free
- **Location:** `src/Engine/Core/System/Memory.cpp:62-65,95`
- **Issue:** `WAllocator::Reallocate` calls `std::realloc(ptr, size)` then `std::free(ptr)` — double free (realloc already frees the old block). `ReallocateAligned` frees `_aligned_malloc` memory with `std::free` on Windows — UB/crash (must use `_aligned_free`). The aligned path is used by VMA via `VulkanReallocate`.
- **Fix:** Return the `realloc` result directly; use `_aligned_free` on Windows.

### 5. Controller action edges read the keyboard buffer
- **Location:** `src/Engine/Core/System/Haptic/SDL/SDLFetching.cpp:204`
- **Issue:** In the `sense.controllerCheck` branch, `wasDownLastFrame` is set from `ctx.rawKeys[1][(sizeT)sense.key[0]]` instead of `ctx.rawController[1][(sizeT)sense.controller]`. Controller-only actions get just-pressed/held state polluted by an unrelated keyboard key (default `WKey::N0`).
- **Fix:** Use `rawController[1][sense.controller]`.

### 6. Trigger/joystick deltas are sign-inverted
- **Location:** `src/Engine/Core/System/Haptic/SDL/SDLFetching.cpp:239,242,289,292`
- **Issue:** Deltas computed as `rawX[1] - rawX[0]`. After `AdvanceBelts`, index `[0]` is the current frame and `[1]` the previous, so this is `previous - current` (inverted). Affects `lTriggerDelta`, `rTriggerDelta`, `lJoyDelta`, `rJoyDelta`.
- **Fix:** Use `[0] - [1]`.

### 7. View matrix is never updated
- **Location:** `src/Engine/Core/Handlers/RenderHandler.h:210` / `RenderHandler.cpp:308,359`
- **Issue:** `m_viewMatrix` is declared and read but never assigned. Currently masked because all `DRAWCALL_` drawing is commented out; the moment it's re-enabled, everything renders with an identity view matrix.
- **Fix:** Rebuild `m_viewMatrix` from the camera each frame.

### 8. `MatrixBase` arithmetic operators are broken
- **Location:** `src/Engine/Math/Matrices/MatrixBase.h:28-73`
- **Issue:** `operator*(const MatrixBase&)` is a `const` method that writes to `m_vals` (won't compile when instantiated) and returns a default-constructed, uninitialized `result`. `operator+`/`operator-`/`operator* scalar` assign a `float` to a `std::array<float, ColumnCount>` (won't compile). Template-latent failures.
- **Fix:** Write into `result` (non-const), initialize it, and fix the array/scalar assignments.

---

## 🩷 Performance (with proof and trivial mitigation)

### 9. Frame limiter sleep is dead — CPU spins / uncapped editor
- **Location:** `src/Engine/Core/Engine.cpp:331`, `src/Editor/Core/Editor.cpp:161`
- **Issue:** `sleepMs = frameEnd.count() / (1000 * 1000)` — `frameEnd.count()` is microseconds, dividing by 1,000,000 yields seconds, not milliseconds. At a 120 FPS cap the remaining ~5 ms becomes `sleepMs = 0`, so `SDL_Delay` never sleeps and the engine burns a core in the spin loop (~60% of one core at 120 FPS). The editor has the same bug but no spin loop, so it runs fully uncapped at 100% CPU.
- **Fix:** Divide by `1000` (µs → ms). One-liner.

### 10. Every render mission is sorted and drawn twice
- **Location:** `src/Engine/Core/Handlers/RenderHandler.cpp:118-119`
- **Issue:** `SortMissions(false)` then `SortMissions(true)` both walk the full `m_renderQueue`, and the transparency filter inside is commented out → every non-stationary mission is inserted into `m_sortedMissions` twice → 2× draw calls / overdraw per frame.
- **Fix:** Drop the second call (or restore the filter).

---

## 🟡 Logic / API / maintainability

### 11. Texture copy regions ignore creation-time extent rounding
- **Location:** `src/Engine/Core/System/Iris/Vulkan/IrisResources.cpp:100-103` / `IrisCommands.cpp:243-258`
- **Issue:** `CreateTexture` rounds the image extent up to a multiple of 4, but `CopyBufferToTexture` derives mip region extents from the unrounded `tex.width/height` and halves them down to 1. For non-multiple-of-4 dimensions the regions underfill the image (uninitialized data), and for BC formats the 2x2/1x1 mip regions aren't block-aligned → validation errors.
- **Fix:** Compute regions from the rounded dimensions; clamp mips to block multiples.

### 12. `CreateGraphicsPipeline` ignores its format fields
- **Location:** `src/Engine/Core/System/Iris/Vulkan/IrisResources.cpp:534-540`
- **Issue:** `desc.colorAttachmentFormat`/`depthStencilFormat` are ignored; the pipeline is always built against `FindBestSwapchainFormat()`/`FindBestDepthFormat()`.
- **Fix:** Use the desc values (or drop them from the API).

### 13. Depth compare op derived from the blend flag
- **Location:** `src/Engine/Core/System/Iris/Vulkan/IrisResources.cpp:454-467`
- **Issue:** Blending on → `LESS_OR_EQUAL`, off → `LESS`. The depth function should come from `desc.depthStencil.depthCompareOp`.
- **Fix:** Use the depth stencil desc.

### 14. Push constants stage mismatch
- **Location:** `src/Engine/Core/System/Iris/Vulkan/IrisBinding.cpp:95`
- **Issue:** `vkCmdPushConstants` always uses `VK_SHADER_STAGE_ALL_GRAPHICS`, but the pipeline layout range may declare a subset of stages → validation error. The layout also gets a push-constant range even when `pushConstantsSize == 0`.
- **Fix:** Use the pipeline's declared push-constant stage/size.

### 15. Index type hardcoded to UINT32
- **Location:** `src/Engine/Core/System/Iris/Vulkan/IrisBinding.cpp:189`
- **Issue:** `vkCmdBindIndexBuffer` hardcodes `VK_INDEX_TYPE_UINT32`; the API has no index-type parameter. 16-bit index buffers will mis-render.
- **Fix:** Add an index type to the Iris API.

### 16. Vertex attribute offsets packed across bindings
- **Location:** `src/Engine/Core/System/Iris/Vulkan/IrisResources.cpp:501-511`
- **Issue:** Attribute offsets are auto-computed by accumulating format sizes across *all* bindings, never restarted per binding, and there's no `offset` field. Multi-binding layouts (e.g., per-instance data) get wrong offsets.
- **Fix:** Reset the offset counter per binding.

### 17. SPIR-V bytecode booked as VRAM
- **Location:** `src/Engine/Core/System/Iris/Vulkan/IrisResources.cpp:209`
- **Issue:** Shader bytecode is host memory, not VRAM; the VRAM widget over-reports.
- **Fix:** Don't add `bytecodeSize` to `vramStats.total`.

### 18. ImGui sampler never initialized
- **Location:** `src/Engine/Core/System/Iris/Vulkan/Helpers/ImGui.cpp` / `IrisImGui.cpp:79`
- **Issue:** `SetupImGuiSampler()`/`SetupImGuiDescriptorPool()` are dead code; `imGuiSampler` stays `VK_NULL_HANDLE`. `TextureToImGui` would pass a null sampler to `ImGui_ImplVulkan_AddTexture` (latent crash/validation error; currently unused).
- **Fix:** Call `SetupImGuiSampler()` during `ConfigureImGui`.

### 19. Swapchain format chosen without checking the surface
- **Location:** `src/Engine/Core/System/Iris/Vulkan/Helpers/Swapchain.cpp:33-36`
- **Issue:** Surface formats are queried (`formats`/`fmtCount`) but never used; `FindBestSwapchainFormat` checks device format-properties only, so `vkCreateSwapchainKHR` can fail on surfaces that don't support `B8G8R8A8_UNORM`.
- **Fix:** Validate against the queried surface formats.

### 20. `std::aligned_alloc` requires size to be a multiple of alignment
- **Location:** `src/Engine/Core/System/Memory.cpp:35-47,73-101`
- **Issue:** VMA sizes are frequently not a multiple of the requested alignment → UB per the C standard (glibc lenient, musl isn't).
- **Fix:** Round the size up to a multiple of alignment.

### 21. Texture upload bucketing can never finish
- **Location:** `src/Engine/Core/Handlers/AssetRepo.cpp:670-687`
- **Issue:** `FillCopyBuffers` requires an exact width match against {128,256,512,1024,2048}, but the header comments say "for 128 or lower" etc. Any texture outside that set (64, 4096, 1000, …) is never uploaded, `TickTextureUpload` never completes, and an empty copy pass is submitted every frame forever. Non-square textures sharing a width can also exceed their bucket's buffer size and get silently truncated.
- **Fix:** Bucket by "width <= X"; size buffers for the worst-case non-square mip chain.

### 22. Texture format hardcoded to BC1
- **Location:** `src/Engine/Core/Handlers/AssetRepo.cpp:654`
- **Issue:** `desc.format = Iris::ImgFormat::BC1` ignores `dds.format` (BC1/4/5). A BC4/5 texture is created as BC1 → garbage pixels.
- **Fix:** Use `IrisImgFormatFromBC(dds.format)`.

### 23. Mesh payload copy trusts the header counts
- **Location:** `src/Engine/Core/Handlers/AssetRepo.cpp:611-613`
- **Issue:** `vertCount*32 + indCount*4 + 20` bytes are memcpy'd without validating against the extracted file size → OOB read on a malformed package.
- **Fix:** Bounds-check against the file size.

### 24. Duplicate dead check
- **Location:** `src/Engine/Core/Handlers/AssetRepo.cpp:509-514`
- **Issue:** `meshPackTable` existence is checked twice; the first check is dead.
- **Fix:** Remove the duplicate.

### 25. ImGui theme applied to a discarded copy
- **Location:** `src/Engine/Core/Handlers/RenderHandler.cpp:586-613`
- **Issue:** `auto colors = style.Colors;` copies the palette, all theme edits modify the copy, and it's never written back → the entire custom theme is dead code.
- **Fix:** Write into `style.Colors` directly.

### 26. Non-`init` SectorLogic instances leak
- **Location:** `src/Engine/Core/World/Sector.cpp:281-297`
- **Issue:** A `SectorLogic` is constructed for every logic entry, but only `init`-bounded ones are executed and destroyed.
- **Fix:** Store/destroy them consistently.

### 27. Audio `currentSample` data race
- **Location:** `src/Engine/Core/Handlers/AudioHandler.cpp:20-37,80-85`
- **Issue:** The SDL audio thread reads/writes `player->currentSample` while `SwapAudioClip` writes it from the game thread, with no synchronization.
- **Fix:** Guard with an atomic or lock.

### 28. Every physics body gets a hardcoded 0.1 cube
- **Location:** `src/Engine/Core/Handlers/PhysicsHandler.cpp:32-37`
- **Issue:** `CreateBody` attaches `b3MakeCubeHull(0.1f)` to every body before any shape component runs.
- **Fix:** Don't add a default shape.

### 29. Non-void stubs without a return (UB if called)
- **Location:** `src/Engine/Core/System/Haptic/SDL/SDLHelpers.cpp:21-24,68-72`; `src/Engine/Core/System/HapticSDL.cpp:100-153`
- **Issue:** `StringToWMouseBtn`, `ValidateActionRequest`, `Haptic::GetPatchList`, `Haptic::GetVendor` fall off the end without returning a value.
- **Fix:** Return defaults.

### 30. Controller-connect path leaks and can null-deref
- **Location:** `src/Engine/Core/System/Haptic/SDL/SDLCore.cpp:122-130`
- **Issue:** `SDL_GetGamepads` result is never `SDL_free`d, and `SDL_OpenGamepad`'s null result is used unguarded.
- **Fix:** `SDL_free` the array; check for null.

### 31. Input-map parse errors still push broken senses
- **Location:** `src/Engine/Core/Handlers/Input.cpp:124-131`
- **Issue:** `ParseAction`/`ParseFloat`/`ParseVector` return on missing fields, but `ParseSingleInput` pushes the sense anyway; a malformed Float/Vector silently becomes a dead Action sense.
- **Fix:** Propagate the failure.

### 32. `Math::Atan2` parameter names are swapped
- **Location:** `src/Engine/Math/Functions/Trigonometry.cpp:38-41`
- **Issue:** `Atan2(x, y)` forwards to `atan2f(x, y)` — the first argument is treated as `y`. Current callers pass `(numerator, denominator)` so it works, but it's a foot-gun.
- **Fix:** Rename parameters to `(y, x)` or swap the forwards.

### 33. `Vector3::operator<` compares by magnitude
- **Location:** `src/Engine/Math/Vectors/Vec3f.h:53-56`
- **Issue:** Not a strict weak ordering (distinct equal-magnitude vectors compare "equal") → UB if used in `std::set`/`std::map`/`std::sort`.
- **Fix:** Only use for magnitude sorting, or implement lexicographic ordering.

### 34. `MatrixMultiply` returns uninitialized data for non-square inputs
- **Location:** `src/Engine/Math/Matrices/MatrixMath.h:20-21`
- **Issue:** Non-square result returns a default-constructed (uninitialized) matrix. Also `Dot<m2Rows>` requires `m1Columns == m2Rows` (compile error otherwise).
- **Fix:** Zero-initialize and enforce/validate dimensions.

### 35. `Freecam` uses a static `firstFrame`
- **Location:** `src/Game/Components/Freecam.cpp:26`
- **Issue:** `static bool firstFrame` is shared across all instances — only the first instance ever gets the initial skip.
- **Fix:** Make it an instance member.

### 36. Debug log noise on copy-buffer creation
- **Location:** `src/Engine/Core/System/Iris/Vulkan/IrisRecording.cpp:86-87`
- **Issue:** Logs the fence status on every `CreateCopyBuffer` call.
- **Fix:** Remove.

### 37. Usage-bitmask stats switch misses combined usages
- **Location:** `src/Engine/Core/System/Iris/Vulkan/IrisResources.cpp:47-65` (and `DestroyBuffer`)
- **Issue:** `BufferUsage` is a bitmask (`Vertex | TransferDst` etc.), but the `switch` only matches single-bit values → combined usages aren't counted in VRAM stats.
- **Fix:** Use bitwise checks.

### 38. `minImageCount = capabilities.minImageCount + 1` can exceed max
- **Location:** `src/Engine/Core/System/Iris/Vulkan/Helpers/Swapchain.cpp:42`
- **Issue:** If `minImageCount == maxImageCount`, the +1 overflows the supported range → validation error (rare).
- **Fix:** Clamp to `maxImageCount`.

---

## 🟢 Convention violations (naming / consistency)

| Location | Issue |
|----------|-------|
| `src/Engine/Core/World/Sector.cpp:87` | `size_t` loop index — must be `sizeT` (the only first-party `size_t`; allocator/curl `size_t` are API-forced). |
| `src/Engine/Core/System/Iris/Vulkan/Helpers/Helpers.cpp:509-515` | `int count_extensions`, `int i` for counting / loop indices. |
| `src/Engine/Core/System/Iris/Vulkan/Helpers/Queues.cpp:17,28,68,70` | `int i`, `int j` loop indices. |
| `src/Engine/Core/System/Iris/Vulkan/IrisRecording.cpp:25`, `Helpers/CmdBuffs.cpp:16` | `uint32` loop indices — use `sizeT`. |
| `src/Engine/Core/Handlers/AssetRepo.cpp:672` | `uint32 handleCursor` counter — use `sizeT`. |
| `src/Engine/Core/System/Iris/Vulkan/IrisCommands.cpp:234` | `std::vector` instead of `wtl::vector`. |
| `src/Engine/Core/Handlers/RenderHandler.cpp:93,111` | `int frame = ...` assigned but never used. |
