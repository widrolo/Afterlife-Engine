#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in mat4 inModel;

layout(push_constant) uniform PushConstants {
    mat4 vp;
} pc;

layout(location = 1) out vec3 outWorldPos;

void main()
{
    outWorldPos = vec3(inModel * vec4(inPosition, 1.0));
    gl_Position = pc.vp * inModel * vec4(inPosition, 1.0);
}
