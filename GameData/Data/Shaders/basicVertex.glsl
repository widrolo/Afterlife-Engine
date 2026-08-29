#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 uv0;

layout(push_constant) uniform PushConstants {
    mat4 mvp;
    mat4 model;
} pc;

layout(location = 1) out vec2 outUV0;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec3 outWorldPos;

void main()
{
    outUV0 = vec2(uv0.x, 1.0 - uv0.y);

    outNormal = mat3(pc.model) * inNormal;
    outWorldPos = vec3(pc.model * vec4(inPosition, 1.0));

    gl_Position = pc.mvp * vec4(inPosition, 1.0);
}
