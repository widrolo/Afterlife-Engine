#version 460

layout(location = 0) in vec3 inWorldPos;
layout(location = 1) flat in int inColorIdx;

layout(set = 0, binding = 0) uniform PhysicsColors
{
    vec4 c[3];
} phyCols;

layout(location = 0) out vec4 outColor;

void main()
{
    vec3 normal = normalize(cross(dFdy(inWorldPos), dFdx(inWorldPos)));
    if (!gl_FrontFacing)
        normal = -normal;

    const vec3 lightDirection = normalize(vec3(0.35, -0.8, 0.45));
    float lighting = 0.2 + 0.8 * max(dot(normal, lightDirection), 0.0);
    vec3 col = phyCols.c[inColorIdx].xyz;
    outColor = vec4(lighting * col, 1.0);
}
