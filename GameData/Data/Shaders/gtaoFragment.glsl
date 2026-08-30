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
    float z = texture(depth, uv).x;
    vec2 ndcXY = uv * 2.0 - 1.0;
    float ndcZ = z * 2.0 - 1.0;

    vec4 viewPos = settings.invProj * vec4(ndcXY, ndcZ, 1.0);
    viewPos /= viewPos.w;

    return (settings.invView * viewPos).xyz;
}
//
//void main()
//{
//    vec3 pos = ReconstructWorldPosition(inUV);
//    vec3 normal = texture(normals, inUV).xyz * 2.0 - 1.0;
//    normal = normalize(normal);
//
//    vec3 toCamera = normalize(settings.camPos - pos);
//    float d = dot(normal, toCamera);
//
//    if (d < 0.0)
//    {
//        outColor = vec4(1.0, 0.0, 0.0, 1.0);
//    }
//    else
//    {
//        outColor = vec4(0.0, 1.0, 0.0, 1.0);
//    }
//}

void main()
{
    vec3 p = ReconstructWorldPosition(inUV);

    vec3 n = texture(normals, inUV).xyz;

    vec3 cp = settings.camPos;
    cp.y = - cp.y;
    vec3 toCamera = normalize(cp - p);

    float d = dot(n, toCamera);

    // Visualize the actual dot product.
    outColor = vec4(
        d * 0.5 + 0.5,
        d * 0.5 + 0.5,
        d * 0.5 + 0.5,
        1.0
    );
}