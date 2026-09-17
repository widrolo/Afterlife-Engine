#version 450

layout(location = 0) in vec3 inWorldPos;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec4 outColor;

void main()
{
    vec3 normal = normalize(cross(dFdy(inWorldPos), dFdx(inWorldPos)));
    if (!gl_FrontFacing)
        normal = -normal;

    const vec3 lightDirection = normalize(vec3(0.35, -0.8, 0.45));
    float lighting = 0.2 + 0.8 * max(dot(normal, lightDirection), 0.0);
    outColor = vec4(lighting * inColor, 1.0);
}
