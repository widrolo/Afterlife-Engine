---
description: Rechecks and updates API.md against the current public headers of a core system (e.g. /update Iris, /update RenderHandler, or /update all).
agent: build
---

Update API.md to match the current code of the requested system.

Target system: $ARGUMENTS

## Mission

You are the maintainer keeping API.md in sync with the public headers. Read the system's public header(s) from disk, extract the public API, and rewrite the system's section in API.md so that every public API member is documented accurately and nothing non-public leaks in.

## System lookup

The argument names the section to update. Resolve it to its header(s):

| Argument | System | Header(s) |
| --- | --- | --- |
| `Iris` / `iris` | Iris | `src/Engine/Core/System/Iris.h` |
| `Haptic` / `haptic` | Haptic | `src/Engine/Core/System/Haptic.h` |
| `OS` / `os` | OS | `src/Engine/Core/System/OS.h` |
| `WAllocator` / `Memory` / `allocator` | WAllocator | `src/Engine/Core/System/Memory.h` |
| `ThreadScheduler` | ThreadScheduler | `src/Engine/Core/System/ThreadScheduler.h` |
| `GPUSettings` | GPUSettings | `src/Engine/Core/System/GPUSettings.h` |
| `RenderHandler` | Render Handler | `src/Engine/Core/Handlers/RenderHandler.h` |
| `AudioHandler` | Audio Handler | `src/Engine/Core/Handlers/AudioHandler.h` |
| `Input` / `input` | Input | `src/Engine/Core/Handlers/Input.h` |
| `WidgetHandler` | Widget Handler | `src/Engine/Core/Handlers/WidgetHandler.h` |
| `RNGHandler` | RNG Handler | `src/Engine/Core/Handlers/RNGHandler.h` |
| `PhysicsHandler` | Physics Handler | `src/Engine/Core/Handlers/PhysicsHandler.h` |
| `TimeHandler` | Time Handler | `src/Engine/Core/Handlers/TimeHandler.h` |
| `JobHandler` | Job Handler | `src/Engine/Core/Handlers/JobHandler.h` |
| `AssetRepo` | Asset Repo | `src/Engine/Core/Handlers/AssetRepo.h` |
| `all` / empty | Everything | All of the above |

If the argument is empty or `all`, update every section listed above. If the argument does not match any system, list the valid names from the table and stop without editing.

## What counts as public API

- Everything in a `public:` access section of a class/struct. Never include anything under `private:` or `protected:`.
- For namespace-scoped APIs (e.g. `namespace Iris { ... }`) there are no access specifiers: everything in the namespace is public. But skip free functions/structs whose only job is backend plumbing if they are not exposed for callers (check: is it usable by game code? if unsure, ask yourself whether a caller outside the system would ever call it).
- Functions, methods, static functions, templates and their default template arguments, member variables (documented as fields with their default values), structs, enums, typedefs, and macros.
- Default argument values (e.g. `QueueType queue = QueueType::Graphics`) belong in the signature shown in API.md.
- Fixed-width types, `sizeT`, `wtl::` containers: keep the exact type spelling from the header, honoring AGENTS.md conventions. Do not rewrite `sizeT` to `size_t`.

## What never goes in API.md

- Private/protected members, helper functions, and internal structs (e.g. private `ModelGroup`/`StationaryObjStaged` in RenderHandler, private `ASMFHeader` in AssetRepo, private fields in `m_...` blocks).
- Backend-specific translation unit details (e.g. the SDL backend of Haptic).
- Stub internals: if a documented member is declared but the implementation is a stub, keep it in API.md but reflect the stub truthfully in the description, just like existing entries ("stubbed", "does nothing for now", etc.).
- Doxygen comment bodies are descriptions, not API members. Do not copy comment formatting or `@param` text into API.md.

## Procedure for each system

1. Read the system's header(s) from disk. Never trust memory; the header on disk is the source of truth.
2. Read the current section for that system in API.md.
3. Extract every public API member from the header.
4. Diff against the current API.md section and reconcile every member:
   - **Signature changed?** Update the `| Function | Description |` row to the new signature and re-derive the description.
   - **Removed?** Delete its row.
   - **Not documented?** Add a row.
   - **Description stale or wrong?** Rewrite it. The description must still be valid against the current code — re-read the declaration (and if needed the implementation) to confirm what it actually does. Note comments/Doxygen in the header are a hint; if the code contradicts them, the code wins.
5. Keep API.md's existing structure for the section: the `### System (path)` heading and, for Iris, the `#### ...` subsection grouping. Reuse existing table wording style; it is casual and shorthand (e.g. "Draws the frame.").
6. Write the updated section back into API.md. Edit only the target system's section(s) and preserve the preamble and every other section byte-for-byte.

## Constraints

- Only edit `API.md`. Never modify the headers or any other source file.
- Never add comments or documentation to header files.
- Do not touch the `// This file does not contain LLM generated documentation` note in any header.
- Use the exact type/function spelling from the header (fixed-width types, `wtl::`, `sizeT`).
- When done, report a concise summary of what changed per system (added / removed / reworded) and explicitly state "No changes needed" for systems where nothing was out of date.
