#version 450

layout(location = 0) in vec2 inUV;

layout(set = 0, binding = 0) uniform sampler2D tex;
layout(set = 1, binding = 0) uniform sampler2D ao;

layout(location = 0) out vec4 outColor;

void main()
{
    float aoMul = texture(ao, inUV).x;
    outColor = texture(tex, inUV) * aoMul;
}