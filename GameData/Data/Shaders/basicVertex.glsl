#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 uv0;

//layout(location = 5) in mat4 inModel;

layout(push_constant) uniform PushConstants {
    mat4 mvp;
} pc;

layout(location = 1) out vec2 outUV0;
//layout(location = 3) out vec3 outNormal;
//layout(location = 4) out vec3 outFragPos;

void main()
{
    outUV0 = uv0;

    //outNormal = mat3(inModel) * inNormal;

    //gl_Position = pc.vp * inModel * vec4(inPosition, 1.0);
    gl_Position = pc.mvp * vec4(inPosition, 1.0);
    //outFragPos = vec3(inModel * vec4(inPosition, 1.0));
}
