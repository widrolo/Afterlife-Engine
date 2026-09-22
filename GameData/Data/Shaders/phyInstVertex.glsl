#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 1) in mat4 inModel;

layout(push_constant) uniform PushConstants {
    mat4 vp;
} pc;

layout(location = 0) out vec3 outWorldPos;
layout(location = 1) out int outColorIdx;

void main()
{
    outColorIdx = 0;
    outWorldPos = vec3(inModel * vec4(inPosition, 1.0));
    gl_Position = pc.vp * inModel * vec4(inPosition, 1.0);
}
