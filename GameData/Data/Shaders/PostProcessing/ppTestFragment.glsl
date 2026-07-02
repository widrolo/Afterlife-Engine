#version 450

layout(location = 0) in vec2 inUV;

layout(set = 0, binding = 0) uniform sampler2D tex;

layout(location = 0) out vec4 outColor;

const float exposure = 0.35;

// vibe coded for now, i just want something. TODO: make it better

vec3 RRTAndODTFit(vec3 v)
{
    vec3 a = v * (v + 0.0245786) - 0.000090537;
    vec3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
    return a / b;
}

vec3 ACESFitted(vec3 color)
{
    const mat3 ACESInputMat = transpose(mat3(
        0.59719, 0.35458, 0.04823,
        0.07600, 0.90834, 0.01566,
        0.02840, 0.13383, 0.83777
    ));

    const mat3 ACESOutputMat = transpose(mat3(
         1.60475, -0.53108, -0.07367,
        -0.10208,  1.10813, -0.00605,
        -0.00327, -0.07276,  1.07602
    ));

    color = ACESInputMat * color;
    color = RRTAndODTFit(color);
    color = ACESOutputMat * color;

    return clamp(color, 0.0, 1.0);
}

void main()
{
    vec3 inCol = texture(tex, inUV).xyz;

    inCol *= exposure;

    vec3 ldr = ACESFitted(inCol);

    ldr = pow(ldr, vec3(1.0 / 2.2));

    outColor = vec4(ldr, 1.0);
}