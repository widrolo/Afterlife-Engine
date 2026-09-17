#version 450

layout(location = 0) in vec3 inPosition;

layout(push_constant) uniform PushConstants {
    mat4 mvp;
    mat4 model;
} pc;

layout(location = 0) out vec3 outWorldPos;
layout(location = 1) out vec3 outColor;

void main()
{
    outColor = vec3(0.0, 1.0, 0.0);
    outWorldPos = vec3(pc.model * vec4(inPosition, 1.0));
    gl_Position = pc.mvp * vec4(inPosition, 1.0);
}
