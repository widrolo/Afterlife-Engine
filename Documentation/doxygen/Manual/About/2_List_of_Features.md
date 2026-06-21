@page list_features About

# List of Features

## Platforms
As of now, only these platforms are supported:

- Windows (64-bit)
- Linux (64-bit)

However, adding a new platform is generally trivial. The engine framework is designed to never use system specific functions directly but rather their abstractions. If you add a new platform to the abstractions, and write the functions identically to the base platforms, then the game will run identically.

## Rendering
Widrolo Engine primarily uses Forward rendering with Vulkan as the primary rendering API. All rendering inside of the engine is accessed via Iris, the abstraction layer used for rendering.

Note that the rendering is still in its early stages.

## Audio
Audio is okay, nothing much. It can play .wav files only for now, but it will be better in the future. Soon music will be in ogg.

## Input
Input is the most mature part of the engine right now. It supports keyboards, mice, controllers and even steeing wheels. 

## GUI
GUI is currently handled by ImGui, even for game UI. But a custom game UI sollution is in the works