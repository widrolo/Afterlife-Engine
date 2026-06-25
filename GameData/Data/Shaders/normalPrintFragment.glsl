#version 450

layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec4 outColor;

void main()
{
    vec3 n = normalize(inNormal);
    vec3 colNorm = n * 0.5 + 0.5;
    outColor = vec4(colNorm, 1.0);
}