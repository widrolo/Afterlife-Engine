@page introduction2 Getting Started

# Introduction to Widrolo Engine

If you still are undecided if you want to use a battle tested indie game engine like Unity or Godot, or rather an underground hobby engine like Widrolo Engine, then fear not: this page will sure convince you to not pick Widrolo Engine! All jokes aside, Widrolo Engine is still a usable engine for specific games. 

You must understand that its appearance as an general purpose engine is only an illusion. It imposes very strict restrictions that are meant to enhance the quality of games by literally forcing the programmer/artist to do more work.

One such example is the complete lack of rotation. The engine does offer trionometric functions, but rotation is completely disallowed. This is to avoid making lazy rotations: lets say you have a spinning fireball. Using Unity you would instincively just rotate it while its flying. Widrolo Engine forces you to draw a sprite for each angle of rotation, theoretically enhancing art quality.

## Game Development Pipeline

### World creation
The main tool used for making games is WEDGE (Widrolo Enhanced Development Game Editor). Wedge is the tool that gives Widrolo Engine a face, allowing for the editing of sectors. It also contains some other useful tooling.

### Models
While the engine currently uses GLB for 3D meshes, a new, engine specific file format is in the planning phase.

### Textures
Widrolo Engine only supports the PNG file format. However, there is a catch to it. Your file extensions can be ".png" and ".pbr". That is completely optional and only meant for sanity checking during material compilation. the PBR file extention is not custom; it is just a renamed PNG file. More on that in the material section.

### Audio
I wouldnt bother with audio right now. If you must, then only WAV is supported. But i will integrate FMOD soon, so just wait for that.

### Programming
Its just C++. Nope, nothing else. Yes, you can integrate Lua if youre brave.


