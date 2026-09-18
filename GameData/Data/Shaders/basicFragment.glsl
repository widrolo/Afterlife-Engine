#version 450

#include "Lighting.glsl"

layout(location = 1) in vec2 inUV0;
layout(location = 2) in vec3 inNormal;

layout(set = 0, binding = 0) uniform sampler2D tex;

layout(set = 1, binding = 0, std140) uniform WorldLightingBlock
{
    WorldLighting lighting;
} world;
layout(set = 2, binding = 1) uniform RenderSettings
{
    vec3 camPos;
    mat4 invProj;
    mat4 invView;
    vec2 viewSize;
} render;

layout(location = 0) out vec4 outColor;

vec3 CalcDiffuse(vec3 normal, vec3 lightDir)
{
    float diff = max(dot(normal, lightDir), 0.0);
    return diff * world.lighting.sun.color.xyz;
}

void main()
{
    vec3 n = inNormal;
    n.y = -n.y;
    n = n * 0.5 + 0.5;

    vec3 lightDir = normalize(world.lighting.sun.direction);

    vec3 diffuse = min(CalcDiffuse(n, lightDir), 1.0);
    vec3 ambient = world.lighting.ambient.xyz * world.lighting.ambIntensity;

    vec3 lightCalc = ambient + diffuse;
    outColor = texture(tex, inUV0) * vec4(lightCalc, 1.0);
}
