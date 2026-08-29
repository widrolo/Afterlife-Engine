#version 450

layout(location = 0) in vec2 inUV;

layout(set = 0, binding = 0) uniform sampler2D depth;
layout(set = 1, binding = 0) uniform sampler2D normals;
layout(set = 2, binding = 0) uniform GTAOSettings
{
    float someVal;
} settings;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(0.0, 0.0, 0.0, 1.0);
}