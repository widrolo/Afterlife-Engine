@page introduction2 Getting Started

# Introduction to Widrolo Engine

If you are still undecided if you want to use a battle-tested indie game engine like Unity or Godot, or rather an underground hobby engine like Widrolo Engine, then fear not: this page will surely convince you to not pick Widrolo Engine! All jokes aside, Widrolo Engine is still a usable engine for specific games.

You must understand that its appearance as a general purpose engine is only an illusion. It is meant to make one type of game: a tight open world desert, with third person combat and small story elements, with some very small RPG elements.

That means that if you want a massive open world first person racing game, then forget about it,

## Game Development Pipeline

### World creation
The main tool used for making games is WEDGE (Widrolo Enhanced Development Game Editor). Wedge is the tool that gives Widrolo Engine a face, allowing for the editing of sectors. It also contains some other useful tooling.

### Models
While the engine currently uses GLB for 3D meshes. A new engine-specific file format is in the planning phase.

### Textures
Widrolo Engine only supports the PNG file format. However, there is a catch to it. Your file extensions can be ".png" and ".pbr". That is completely optional and only meant for sanity checking during material compilation. The PBR file extension is not custom; it is just a renamed PNG file. More on that in the materials section.

### Audio
I wouldn't bother with audio right now. If you must, then only WAV is supported. But i will integrate FMOD soon, so just wait for that.

### Programming
It's just C++. Nope, nothing else. Yes, you can integrate Lua if you're brave.
