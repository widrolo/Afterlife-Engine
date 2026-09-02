# Afterlife Engine

> This file is a prompt written by the maintainer to prevent AI hallucinations about the engine. It is loaded automatically at the start of every session. **Update it whenever the engine changes.**

## API Reference

For the public API of the System Abstractions, the Deep/High Level Core Handlers, and the Asset Repo, refer to **API.md** instead of searching through the header and implementation files. API.md is kept in sync with the public headers; update it whenever the public API changes.

## Overview

- 3D open world game engine written in **C++**.
- On PC, uses **Vulkan** for rendering and **SDL** for input.
- Uses **YAML for every configuration**: sectors, shaders, materials, input maps, and even component definitions for the editor.
- **Visual goal**: PS2-style rendering with slightly higher resolution textures. All color textures use **BC1** compression.

## Constraints

- **Load once at boot, never again.** The engine loads all assets at startup and does not stream or load anything later. This deliberately fights scope creep.
- Consequence: all graphical assets for a game must fit within the **lowest common denominator of hardware: a 4 GB GPU**. After accounting for the OS, other applications, and game framebuffers, this means every game can realistically use only **~2.5–3 GB of graphical assets**.

## Architecture Layers

### System Abstractions

Lives in `src/Engine/Core/System/...`

System abstractions are namespaces or classes with **all static functions and no class data**. Their purpose is to provide a common, high-level interface for a part of the system that can be swapped for another implementation via a CMake edit. Implementations live in their own translation units, with dedicated helper functions and global data.

- **Iris**: Rendering. Exposes a low-level, explicitly recorded command API:
  - The caller creates opaque handles for buffers, textures, samplers, shaders (SPIR-V bytecode), resource tables (descriptor sets), and graphics pipelines.
  - Commands are recorded into per-frame command buffers: render passes via dynamic rendering, draw/draw-indexed/draw-indirect, dispatches, and async copy passes.
  - Render targets are framebuffers (color/depth attachments) alongside the swapchain; there is no model/material abstraction: the Render Handler and Asset Repo translate game data into raw buffers and pipelines.
  - Integrates ImGui and books GPU, VRAM, draw, and bind statistics.
  - Currently Vulkan-only. Compute pipelines and compute passes are still stubbed; shutdown is deprecated and left to the driver; buffer-to-texture uploads only accept BC1/4/5 formats.
- **Haptic**: Input and haptic output. An all-static facade with no class data; real state lives in backend-specific translation units behind a static context. The backend is picked by a CMake define (`HAPTIC_BACKEND`, currently the SDL3 backend). Behaves like Steam Input by default.
  - Input maps are YAML files parsed by the Input handler into `InputSense` entries and loaded into Haptic as named maps, one selected at a time. Gameplay queries senses by name and gets Nullable results (empty when missing).
  - Three sense kinds: **Action**, **Float**, **Vector**, each bindable to keyboard, mouse, and/or controller.
  - Per frame, Haptic polls raw device state into double-buffered ValueBelt slots (current vs. previous frame), detects controller hotplug and vendor type, translates raw SDL scancodes/buttons into vendor-neutral `WKey`/`WMouseBtn`/`WPadBtn` enums, then runs signal processing: just-pressed/held/just-released edges for actions, normalized [-1,1] floats (triggers, with delta variants), and 2D vectors (joysticks with a deadzone, WASD-style key clusters).
  - Mouse state is fetched but not yet translated into senses. Output maps, rumble/LED, input patching, and `GetVendor` are declared but still stubbed.
- **OS**: Rarely used, since the STL covers most OS differences; when it doesn't, OS steps in to resolve it.
- **Echo**: Audio. Sounds and streams as resources, virtual audio instances for playback, and buses for grouping and mixing.

### Deep Core Handlers

Lives in `src/Engine/Core/Handlers/...`

Deep core handlers are **singletons stored globally** in the `CoreSystems` class alongside other handlers. They are distinguished only by philosophy: they primarily interact with System Abstractions.

- **Render Handler**: Records render missions during the draw part of the game loop. Performs automatic optimizations before sending work to Iris.
- **Sector Handler**: Owns every sector in the game. Loads them all at boot from the Asset Repo and draws them each frame.
- **Input**: Not really a handler, but an interface to Haptic used by gameplay code. Also parses YAML input maps into `InputSense` entries and loads them into Haptic.

### Asset Repo

Lives in `src/Engine/Core/Handlers/...`

Treated like a handler, but far from it. It is the primary location for getting all kinds of files. It does not usually return files themselves (unless instructed), but rather partially parsed versions, e.g. instead of 3D binary data, it hands back vertex data.

- Communication is usually done by creating an **asset mission** with just a name, passing it into `GetAsset()` by reference. The asset repo fills out the mission, which the caller can then use; the return type remains `void`.
- Asset communication (special channels that can do extra work) is only permitted for system abstractions.

### High Level Core Handlers

Lives in `src/Engine/Core/Handlers/...`

Also stored globally in `CoreSystems`, but closer to gameplay. They may touch System Abstractions, but do not rely on them.

- **Widget Handler**: Manages engine and game widgets that use ImGui.
- **RNG Handler**: Rolls RNG. Soon to be deprecated.
- **Physics Handler**: Simulates physics using Box3D. Box3D is still in alpha, but expected to reach relative maturity as development continues.
- **Time Handler**: Manages in-game time and date. Has settings for time speed and is used for lighting.

### Gameplay Concepts

**This is not ECS. There are no entities and no components anymore.**

- **Sector**: A small chunk of the world, absurdly small: one apartment, one segment of a street. A sector only holds **stationary world objects** (`SectorEntry` list): a mesh, a texture, and a transform per entry, parsed from a YAML file at boot. Sectors have no entities, no components, and no ticking; they just exist and get drawn. Loaded once at boot by the Asset Repo into the Sector Handler, which draws them each frame.
- **SectorEntry**: One stationary object within a sector, kept small enough to fit in a cache line. Resolution of asset names to UIDs happens at boot.
- **Dynamic objects**: Anything that moves or has logic (the player, the camera, enemies) is a plain class written in the game, e.g. `Freecam`. It owns its own transform and is ticked, drawn, and plugged into physics **directly by the Game class**, never by components.
- **Game class**: Executes the game and owns the dynamic objects. Has extensive access to ordering in the game loop: tick, draw, physics, audio, plus the points right before and right after each of them, and many others (see `Game.h`).
- **Game Systems**: Handlers purely for gameplay. They only control gameplay, e.g. a score system or an enemy system.

### Editor (ATK)

ATK (Afterlife ToolKit) is an interface for visually editing YAML configurations, centered around editing sectors but containing tools to edit all game data. It is functionally split from the engine but is "like a cousin": it runs many engine core handlers, letting it use Widgets for its UI, and the render handler with Iris for rendering the viewport.

## Code Conventions

### Types & Core

- **Fixed-width types only**: `uint8`/`int8`, `uint16`/`int16`, `uint32`/`int32`, `uint64`/`int64`. Never use built-ins like `int`, `unsigned`, `long`, or `size_t`.
- `sizeT` does **NOT** point to `size_t`. It is an alias for `uint64` and is therefore **always 64 bit** on every platform the engine runs on. Never assume it may be 32 bit. This is a common source of confusion: remember it.
- Extra aliases: `byte` = uint8, `word` = uint16, `dword` = uint32, `qword` = uint64, `float32`, `float64`.
- Min/max constants exist per type: `min_uint32`, `max_float32`, `inf_float32`, `nan_float32`, and so on.
- Size constants: `KB`, `MB`, `GB`, `TB`, `PB`.
- **Global state and constants** are declared with `_GLOBAL_` (= `static inline`), `_GLOBAL_CONST_`, and `_GLOBAL_CEX_` (= constexpr). Used for engine-wide config in `EngineSettings`, `PhysicsSettings`, `TimeSettings`, and `GPUSettings`.
- The engine is 64-bit only. No bitness checks.

### Containers & Memory

- Use **`wtl::` containers** (namespace `wtl`, lowercase): `wtl::vector`, `wtl::deque`, `wtl::list`, `wtl::map`, `wtl::queue`. They are std containers wired to `WAllocator` so all memory is tracked. Avoid raw `std::vector` and friends in engine code.
- **All allocations go through `WAllocator`**: macros `wNew(size)`, `wNewArr(type, count)`, `wFree(ptr)`; construct objects with `WAllocator::Construct<T>(args...)`.

### Naming

- **Classes**: PascalCase (`RenderHandler`, `AssetRepo`).
- **Methods and functions**: PascalCase (`GetRandomInt`).
- **Member variables**: `m_` prefix + camelCase (`m_renderQueue`). Statics use the same prefix (`m_uptime`).
- **Local variables and parameters**: camelCase (`speed`, `dt`).
- **Public struct fields**: no prefix (`meshUID`, `transform` in `RenderMission`).
- **Enums**: `enum class` throughout. Members are PascalCase (`Samples1`) or `X_Y` (`Triangle_List`).
- **Macros**: UPPER_SNAKE (`BIT`, `DEFINE_OPAQUE_HANDLE`).
- **Files**: PascalCase, one class per `.h`/`.cpp` pair matching the class name.
- **Namespaces**: `WEngine` (engine), `WEditor` (editor), `Iris`, `wtl`. Game code often sits in the global namespace (e.g. `Freecam`).

### Headers & Formatting

- `#pragma once` on line 1. No include guards.
- Include paths look like `#include <Engine/...>` (angle brackets; `src` is the include directory).
- **Allman braces**: opening brace on its own line for functions and blocks.
- West `const` (`const std::string&`).
- `using namespace WEngine;` at the top of `.cpp` files, right after the includes.
- `std::format` for building strings; log via `WLog::ConsoleLog` with `WLog::SetConsoleError()`/`SetConsoleWarning()`/etc. for colors.

### Docs & Comments

- **Doxygen** `/** ... */` on public API, using `@param`, `@return`, `@note`, `@warning`, `@tparam`.
- Comments are casual and conversational, sometimes with a lowercase "i" (e.g. `// we aint popping this`).
- Some headers carry the note `// This file does not contain LLM generated documentation`.

### Data (YAML)

- 4-space indentation, lowercase keys, camelCase values.
- `$Sample.yaml` files are used as templates.

### Git

- Informal sentence-style commit messages, no prefixes (e.g. "Fixed command buffer leak", "Black screen renders once more.").
