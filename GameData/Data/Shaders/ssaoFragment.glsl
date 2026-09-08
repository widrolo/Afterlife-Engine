#version 450

layout(location = 0) in vec2 inUV;

layout(set = 0, binding = 0) uniform sampler2D depth;
layout(set = 1, binding = 0) uniform sampler2D normals;
layout(set = 2, binding = 0) uniform RenderSettings
{
    vec3 camPos;
    mat4 invProj;
    mat4 invView;
    vec2 viewSize;
} render;
layout(std140, set = 3, binding = 0) uniform SSAOSettings
{
    float radius;
    float bias;
    float strength;
    float power;
    int sampleCount;
    float falloffStart;
    vec2 padding;
} ssao;

layout(location = 0) out vec4 outColor;

const float PI = 3.14159265359;

vec3 ReconstructViewPosition(vec2 uv, float z)
{
    // Match GTAO: raw Vulkan depth and no UV/NDC Y flip.
    vec4 p = render.invProj * vec4(uv * 2.0 - 1.0, z, 1.0);
    return p.xyz / p.w;
}

void main()
{
    outColor = vec4(1.0);
    ivec2 size = textureSize(depth, 0);
    ivec2 pixel = clamp(ivec2(inUV * vec2(size)), ivec2(0), size - 1);
    float centerDepth = texelFetch(depth, pixel, 0).r;
    if (centerDepth >= 1.0 || ssao.radius <= 0.0 || ssao.strength <= 0.0 || ssao.power <= 0.0)
    {
        return;
    }

    vec2 uv = (vec2(pixel) + 0.5) / vec2(size);
    vec3 p = ReconstructViewPosition(uv, centerDepth);
    vec3 n = texelFetch(normals, pixel, 0).xyz * 2.0 - 1.0;
    // Undo normalsFragment's storage flip before rotating into view space.
    n.y = -n.y;
    n = normalize(transpose(mat3(render.invView)) * n);

    vec3 axis = abs(n.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(0.0, 1.0, 0.0);
    vec3 tangent = normalize(cross(axis, n));
    vec3 bitangent = cross(n, tangent);
    mat4 proj = inverse(render.invProj);
    int sampleCount = clamp(ssao.sampleCount, 1, 64);
    float bias = max(ssao.bias, 0.0);
    float falloffStart = clamp(ssao.falloffStart, 0.0, 0.99);

    // Fixed PCG3D jitter needs neither a noise texture nor temporal history.
    uvec3 noiseBits = uvec3(uvec2(pixel), 0u) * 1664525u + 1013904223u;
    noiseBits.x += noiseBits.y * noiseBits.z;
    noiseBits.y += noiseBits.z * noiseBits.x;
    noiseBits.z += noiseBits.x * noiseBits.y;
    noiseBits ^= noiseBits >> 16u;
    noiseBits.x += noiseBits.y * noiseBits.z;
    noiseBits.y += noiseBits.z * noiseBits.x;
    noiseBits.z += noiseBits.x * noiseBits.y;
    vec3 jitter = vec3(noiseBits >> 8u) * (1.0 / 16777216.0);

    float occlusion = 0.0;
    for (int sampleIndex = 0; sampleIndex < sampleCount; ++sampleIndex)
    {
        // Cosine-weighted hemisphere directions, with more samples near the origin.
        float fraction = (float(sampleIndex) + jitter.y) / float(sampleCount);
        float angle = 2.0 * PI * fract(float(sampleIndex) * 0.61803398875 + jitter.x);
        vec3 direction = sqrt(fraction) * (cos(angle) * tangent + sin(angle) * bitangent)
                         + sqrt(1.0 - fraction) * n;
        float radialNoise = fract(float(sampleIndex) * 0.75487766625 + jitter.z);
        float sampleRadius = ssao.radius * mix(0.1, 1.0, radialNoise * radialNoise);
        vec3 samplePosition = p + direction * sampleRadius;
        vec4 clip = proj * vec4(samplePosition, 1.0);
        if (clip.w <= 0.0)
        {
            continue;
        }
        vec3 ndc = clip.xyz / clip.w;
        vec2 sampleUV = ndc.xy * 0.5 + 0.5;
        if (ndc.z < 0.0 || ndc.z >= 1.0
            || any(lessThan(sampleUV, vec2(0.0))) || any(greaterThanEqual(sampleUV, vec2(1.0))))
        {
            continue;
        }

        // Exact texels avoid interpolated silhouettes and the shared sampler's repeat wrap.
        ivec2 samplePixel = ivec2(sampleUV * vec2(size));
        if (all(equal(samplePixel, pixel)))
        {
            continue;
        }
        float sampleDepth = texelFetch(depth, samplePixel, 0).r;
        if (sampleDepth >= 1.0)
        {
            continue;
        }
        sampleUV = (vec2(samplePixel) + 0.5) / vec2(size);
        vec3 surfacePosition = ReconstructViewPosition(sampleUV, sampleDepth);
        vec3 delta = surfacePosition - p;
        // View space looks down -Z. The normal test rejects coplanar texel-quantization acne.
        if (surfacePosition.z <= samplePosition.z + bias || dot(n, delta) <= bias)
        {
            continue;
        }

        occlusion += 1.0 - smoothstep(falloffStart, 1.0, length(delta) / ssao.radius);
    }

    // Missing/offscreen samples remain visible rather than darkening screen edges.
    float visibility = clamp(1.0 - ssao.strength * occlusion / float(sampleCount), 0.0, 1.0);
    outColor = vec4(vec3(pow(visibility, ssao.power)), 1.0);
}
