# Widrolo Engine 3D

Widrolo Engine 3D is a step up from its predicessor, bringing a Vulkan rendered 3D forard rendered pipeline for creating specifically open world games.

![engine screenshot](/screenshots/Screenshot%20from%202026-06-30%2018-12-18.png)


At this point in time, the engine is rather barren and not really user friendly. In addition to that, its mostly a hobby passion project. In its current state, i would recommend not even bothering with it (hence no license), and just let it marinate until its ready.

The game it is being made for is a tight desert Open World with minor RPG elements. The following list of planned/implemented features will make it clear.

## Current list of features

#### Rendering

Rendering is handled by a Vulkan forawrd renderer, bringing modern simplicity. It does not utilize modern Forward+ rendering, so light limits apply.

A partial PBR rendering pipeline is already in place, offering normal and roughness map support.

Sunlight is implemented, and reacts to the in game time. The Skybox also reacts to the in game time, showing a nice blue sky during the day, and a darker sky during the night.

In addition to that, the engine supports a very custimizable shader/material pipeline, with game size optimization already built in.

#### Sectors/Entities/Components

_Note:_ This system will change dramatically over the coming months.

Sectors are basically "micro scenes" that split up the world granually. They are vital for world streaming due to their miniscule size. The concept alone encourages optimization by design, and drastically helps with reducing draw calls without culling techniques.

Entities are the members of sectors and hold components. This is not ECS. Think of entities like GameObjects in Unity.

Components are the primary gameplay element. They hold values, run logic on entities and communicate with the engine Core. Components are written C++.

#### Input

Widrolo Engine holds a very mature input system that is really fast and intuitive to use. The backend tracks input, so you can easily see whether a key is being held, just pressed, just released and more.

#### Widgets

Widgets are basic ImGui windows that are used for tooling. The engine has a really high priority for tooling, and so Widget creation and management is trivial.

## [W]idrolo [E]nhanced [D]evelopment [G]ame [E]ditor

WEDGE is the primary game editor for the engine. It directly speaks the language of the Engine and dramatically simplifies world building. The interface is built upon the existing ImGui based Widget system of the engine, so making Tools for the editor is as trivial as for the game itself.

## Features Until Release

The following features are currently on high priority and target to be implemented by the end of 2027. Its more of a self TODO list. The order in which these get implemented is more by convenience.

#### Rendering
- [x] Partial PBR Support
- [x] Time of Day Lighting
- [ ] Full PBR Shaders
- [ ] Shadow Maps
- [ ] Ambient Occlusion
- [ ] Post Processing Shaders
- [ ] Tonemapping
- [ ] LOD
- [ ] Mip Maps
- [ ] ImGui Texture Support for Tools

#### Systems
- [ ] Multi Threaded Streaming
- [x] Game Time
- [ ] Sector Collection
- [ ] ECS System for Static Meshes
- [ ] Windows Support (Windows is borked as of now)
- [ ] Custom Mesh File Format

#### Gameplay releated
- [ ] Jolt Physics
- [ ] Behaviour Trees

#### Audio
- [ ] FMOD Support

#### Input
- [ ] Steam Input

#### WEDGE
- [ ] Proper Viewport
- [ ] Material/Shader Editor
- [ ] Undo Support
- [ ] Better Integration with the Game
- [ ] Asset Browser