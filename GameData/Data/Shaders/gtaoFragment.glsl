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
layout(std140, set = 3, binding = 0) uniform GTAOSettings
{
    float radius;
    float falloffStart;
    float strength;
    float power;
    int sliceCount;
    int stepCount;
    float maxRadiusPixels;
    float horizonBias;
} gtao;

layout(location = 0) out vec4 outColor;

const float PI = 3.14159265359;
const float HALF_PI = 0.5 * PI;

vec3 ReconstructViewPosition(vec2 uv, float z)
{
    // Vulkan stores NDC depth directly, even with the current GLM projection.
    // The positive-height viewport also needs no UV/NDC Y flip.
    vec2 ndc = uv * 2.0 - 1.0;
    vec4 p = render.invProj * vec4(ndc, z, 1.0);
    return p.xyz / p.w;
}

void main()
{
    outColor = vec4(1.0);
    ivec2 size = textureSize(depth, 0);
    ivec2 pixel = clamp(ivec2(inUV * vec2(size)), ivec2(0), size - 1);
    float centerDepth = texelFetch(depth, pixel, 0).r;
    if (centerDepth >= 1.0 || gtao.radius <= 0.0 || gtao.strength <= 0.0 || gtao.power <= 0.0)
    {
        return;
    }

    vec2 uv = (vec2(pixel) + 0.5) / vec2(size);
    vec3 p = ReconstructViewPosition(uv, centerDepth);
    vec3 toCamera = normalize(-p);

    vec3 n = texelFetch(normals, pixel, 0).xyz * 2.0 - 1.0;
    // Undo normalsFragment's storage flip, then rotate renderer-world to view space.
    // The camera is the view-space origin, so its gameplay Y flip is not needed here.
    n.y = -n.y;
    n = normalize(transpose(mat3(render.invView)) * n);

    int sliceCount = clamp(gtao.sliceCount, 1, 16);
    int stepCount = clamp(gtao.stepCount, 1, 32);
    float falloffStart = clamp(gtao.falloffStart, 0.0, 0.99);
    float horizonBias = clamp(gtao.horizonBias, 0.0, 1.0);

    // PCG3D gives decorrelated angle/radius jitter without directional stripes.
    // Keep the seed fixed per pixel since there is no temporal filtering.
    uvec3 noiseBits = uvec3(uvec2(pixel), 0u) * 1664525u + 1013904223u;
    noiseBits.x += noiseBits.y * noiseBits.z;
    noiseBits.y += noiseBits.z * noiseBits.x;
    noiseBits.z += noiseBits.x * noiseBits.y;
    noiseBits ^= noiseBits >> 16u;
    noiseBits.x += noiseBits.y * noiseBits.z;
    noiseBits.y += noiseBits.z * noiseBits.x;
    noiseBits.z += noiseBits.x * noiseBits.y;
    vec2 jitter = vec2(noiseBits.xy >> 8u) * (1.0 / 16777216.0);
    float noise = jitter.x;
    float stepNoise = jitter.y;
    float visibility = 0.0;
    vec3 sliceAxis = normalize(vec3(toCamera.z, 0.0, -toCamera.x));
    vec3 sliceAxisPerpendicular = cross(toCamera, sliceAxis);

    for (int slice = 0; slice < sliceCount; ++slice)
    {
        // Uniform angles around the view ray, not the screen normal, avoid off-axis AO bias.
        float angle = PI * (float(slice) + noise) / float(sliceCount);
        vec3 tangent = cos(angle) * sliceAxis + sin(angle) * sliceAxisPerpendicular;
        vec3 screenDirection = tangent * toCamera.z - toCamera * tangent.z;
        vec2 direction = normalize(screenDirection.xy * vec2(size)
                                   / vec2(render.invProj[0][0], render.invProj[1][1]));
        vec3 viewStep = ReconstructViewPosition(uv + direction / vec2(size), centerDepth) - p;

        vec2 projectedNormal = vec2(dot(n, tangent), dot(n, toCamera));
        float normalLength = length(projectedNormal);
        if (normalLength < 0.0001)
        {
            continue;
        }
        float normalAngle = atan(projectedNormal.x, projectedNormal.y);
        float radiusPixels = min(gtao.radius / max(length(viewStep), 0.000001),
                                 max(gtao.maxRadiusPixels, 1.0));
        vec2 horizonCos = vec2(-1.0);

        // Each slice searches both sides for the highest horizon towards the camera.
        for (int side = 0; side < 2; ++side)
        {
            float sideSign = side == 0 ? -1.0 : 1.0;
            for (int step = 0; step < stepCount; ++step)
            {
                float fraction = (float(step) + stepNoise + 0.5) / (float(stepCount) + 0.5);
                float offsetPixels = max(1.0, radiusPixels * fraction * fraction);
                ivec2 samplePixel = ivec2(floor(vec2(pixel) + 0.5 + sideSign * direction * offsetPixels));
                if (any(lessThan(samplePixel, ivec2(0))) || any(greaterThanEqual(samplePixel, size)))
                {
                    break;
                }
                if (all(equal(samplePixel, pixel)))
                {
                    continue;
                }

                // Exact texels avoid interpolated silhouettes and the shared sampler's repeat wrap.
                float sampleDepth = texelFetch(depth, samplePixel, 0).r;
                if (sampleDepth >= 1.0)
                {
                    continue;
                }
                vec2 sampleUV = (vec2(samplePixel) + 0.5) / vec2(size);
                vec3 delta = ReconstructViewPosition(sampleUV, sampleDepth) - p;
                float distanceToSample = length(delta);
                if (distanceToSample <= 0.000001 || distanceToSample >= gtao.radius)
                {
                    continue;
                }

                float weight = 1.0 - smoothstep(falloffStart, 1.0, distanceToSample / gtao.radius);
                float sampleCos = clamp(dot(delta, toCamera) / distanceToSample - horizonBias, -1.0, 1.0);
                horizonCos[side] = max(horizonCos[side], mix(-1.0, sampleCos, weight));
            }
        }

        vec2 horizons = vec2(-acos(horizonCos.x), acos(horizonCos.y));
        horizons = normalAngle + clamp(horizons - normalAngle, -HALF_PI, HALF_PI);

        // GTAO's analytic cosine-weighted integral over the visible arc of this slice.
        vec2 integral = 0.25 * (cos(normalAngle) + 2.0 * horizons * sin(normalAngle)
                               - cos(2.0 * horizons - normalAngle));
        visibility += normalLength * (integral.x + integral.y);
    }

    visibility = clamp(visibility / float(sliceCount), 0.0, 1.0);
    visibility = clamp(1.0 - gtao.strength * (1.0 - visibility), 0.0, 1.0);
    outColor = vec4(vec3(pow(visibility, gtao.power)), 1.0);
}
