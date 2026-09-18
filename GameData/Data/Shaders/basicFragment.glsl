#version 450

#include "Lighting.glsl"

layout(location = 1) in vec2 inUV0;
//layout(location = 3) in vec3 inNormal;

layout(set = 0, binding = 0) uniform sampler2D tex;

layout(set = 1, binding = 0, std140) uniform WorldLightingBlock
{
    WorldLighting lighting;
} world;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = texture(tex, inUV0);
    outColor = vec4(world.lighting.sources[14].data.color);
}
