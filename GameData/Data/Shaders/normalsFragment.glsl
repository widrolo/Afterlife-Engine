#version 450

layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inWorldPos;

layout(location = 0) out vec4 outColor;

void main()
{
    vec3 faceN = cross(dFdx(inWorldPos), dFdy(inWorldPos));
    vec3 meshN = normalize(inNormal);
    if (dot(faceN, meshN) < 0.0)
        faceN = -faceN;

    vec3 n = normalize(faceN);
    vec3 colNorm = n * 0.5 + 0.5;
    outColor = vec4(colNorm, 1.0);
}