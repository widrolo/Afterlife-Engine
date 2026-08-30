#version 450

layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inWorldPos;

layout(location = 0) out vec4 outColor;

void main()
{
    vec3 n = inNormal;
    n.y = -n.y;
    outColor = vec4(n * 0.5 + 0.5, 1.0);
}