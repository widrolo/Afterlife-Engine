
#version 460

layout(location = 0) in vec3 inPosition;

layout(push_constant) uniform PushConstants {
    mat4 mvp;
    mat4 model;
} pc;

layout(location = 0) out vec3 outWorldPos;
layout(location = 1) flat out int outColorIdx;

void main()
{
    outColorIdx = gl_BaseInstance;
    outWorldPos = vec3(pc.model * vec4(inPosition, 1.0));
    gl_Position = pc.mvp * vec4(inPosition, 1.0);
}
