#version 450

layout(location = 1) in vec3 inWorldPos;

layout(location = 0) out vec4 outColor;

void main()
{
    vec3 normal = normalize(cross(dFdy(inWorldPos), dFdx(inWorldPos)));
    if (!gl_FrontFacing)
        normal = -normal;

    normal.y = -normal.y;
    outColor = vec4(normal * 0.5 + 0.5, 1.0);
}
