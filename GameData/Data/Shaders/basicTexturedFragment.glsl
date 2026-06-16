#version 450

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inUV0;

layout(binding = 0) uniform sampler2D tex;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = texture(tex, inUV0) * vec4(inColor, 1.0);
}