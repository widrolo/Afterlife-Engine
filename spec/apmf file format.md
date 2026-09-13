# Afterlife Physics Mesh Format (.apmf)

## Description
Used for tightly packing physics triangle meshes. It is tailor-made for the requirements of pyhsics engines such as Box3D, which is used by Afterlife Engine. This format only stores a single physics mesh, in the exact format that Afterlife Engine expects and needs.

In general, creation of such file is done via the Afterlife Browser. It takes a regular .blend file, extracts the mesh and transforms all vertex information into what's actually needed. This means that even if the format changes in the middle of development, Afterlife Browser can simply recompile all meshes to fit the new format.

This file format is in the general family of the A_MF formats, and may look nearly identical to formats such as ASMF.

### Limitations
The file has no versions. An apmf file from game1 is most likely going to be incompatible with game2. However, this is not a problem since the files are compiled from a source blender scene. Because of this, meshes from game1 can be compiled via the Afterlife Browser version used in game2 for compatability.

#### Output file:
If the input is:
>asset path: `projectPath/Meshes/Table.blend`\
mesh name: `Table_COL`

#### Then the output will be:
>output: `outPath/PhysicsMeshes/Table__Table_COL.apmf`

As we can see, the physics mesh name gets concatenated with a double underscore. This is because no one is naming things with double underscores, so we can safely avoid collisions.

## Format

### File Structure

| Name     |
|----------|
| Header   |
| Vertices |
| Indexes  |

The vertices and indexes are packed tight against each other. In such case, there is zero parsing performed on the data, and is sent directly to a physics engine.\
The format is entirely little endian.

### Header
| Name         | Size    |
|--------------|---------|
| Identifier   | 4 bytes |
| Config Flags | 4 bytes |
| Vertex Count | 4 bytes |
| Index Count  | 4 bytes |

The identifier is the string "APMF". The size of a vertex and index is set, so no size or offset info is needed as it can be calculated.\
Config flags are unused as of now, but they serve alignment.

### Vertex format
| Name     | Size     | Type          |
|----------|----------|---------------|
| Position | 12 bytes | Vec3(float32) |

Triangles must use clockwise index winding.\
The coordinate space is +Y up.

### Index format
Always 32 bit.

## Validation
All validation must be checked in order.

At this point the file must have been closed from writing. The OS must be queried for:
- The file exists.
- The file is accessible.
- The size of the file in bytes must be exactly: header size + (vertex count * vertex size) + (index count * index size)

All validation below must be checked by opening the file in read only mode:
- The file must start with "APMF"
- Vertex count must not be less than 3.
- Index count must not be zero.
- Index count must be a multiple of 3.
- Every vertex is referenced by any index at least once.
- No float can ever be NaN or Inf.
- No index can be numerically higher than (vertex count - 1).

Should any fail, the compilation will stop, the file will be deleted (if file exists) and an appropriate error will print to the console.\
While some of these checks might seem monotonous, they are required simply for the case of proper implementation of the file write.
