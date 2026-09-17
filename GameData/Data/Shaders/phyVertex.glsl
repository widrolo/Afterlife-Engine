#version 450

layout(location = 0) in vec3 inPosition;

layout(push_constant) uniform PushConstants {
    mat4 mvp;
    mat4 model;
} pc;

layout(location = 1) out vec3 outWorldPos;

void main()
{
    outWorldPos = vec3(pc.model * vec4(inPosition, 1.0));
    gl_Position = pc.mvp * vec4(inPosition, 1.0);
}
