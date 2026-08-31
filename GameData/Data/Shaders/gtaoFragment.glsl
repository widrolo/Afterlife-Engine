#version 450

layout(location = 0) in vec2 inUV;

layout(set = 0, binding = 0) uniform sampler2D depth;
layout(set = 1, binding = 0) uniform sampler2D normals;
layout(set = 2, binding = 0) uniform GTAOSettings
{
    vec3 camPos;
    mat4 invProj;
    mat4 invView;
    vec2 viewSize;
} settings;

layout(location = 0) out vec4 outColor;

vec3 ReconstructWorldPosition(vec2 uv)
{
    float z = texture(depth, uv).r;

    vec2 ndc = uv * 2.0 - 1.0;

    vec4 p = settings.invProj * vec4(ndc, z, 1.0);

    return p.xyz / p.w;
}

void main()
{
    vec3 p = ReconstructWorldPosition(inUV);

    vec3 n = texture(normals, inUV).xyz;

    vec3 cp = settings.camPos;
    vec3 toCamera = normalize(cp - p);

    float d = dot(n, toCamera);

    if (d < 0)
    {
        outColor = vec4(-d, 0.0, 0.0, 1.0);
    }
    else
    {
        outColor = vec4(0.0, 0.0, d, 1.0);
    }
}

//void main()
//{
//    vec3 p = ReconstructViewPosition(inUV);
//
//    outColor = vec4(normalize(p) * 0.5 + 0.5, 1.0);
//}