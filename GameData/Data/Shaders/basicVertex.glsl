#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 uv0;
layout(location = 4) in vec2 uv1;

layout(location = 5) in mat4 inModel;

layout(push_constant) uniform PushConstants {
    mat4 vp;
} pc;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outUV0;
layout(location = 2) out vec2 outUV1;
layout(location = 3) out vec3 outNormal;
layout(location = 4) out vec3 outFragPos;

void main()
{
    outColor = inColor;
    outUV0 = uv0;
    outUV1 = uv1;

    outNormal = mat3(inModel) * inNormal;


    gl_Position = pc.vp * inModel * vec4(inPosition, 1.0);
    outFragPos = vec3(inModel * vec4(inPosition, 1.0));
}