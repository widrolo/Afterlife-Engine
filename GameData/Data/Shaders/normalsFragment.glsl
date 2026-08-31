#version 450

layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inWorldPos;

layout(location = 0) out vec4 outColor;

void main()
{
    vec3 n = inNormal;
    n.y = -n.y;
    n = n * 0.5 + 0.5;
    outColor = vec4(n, 1.0);
}