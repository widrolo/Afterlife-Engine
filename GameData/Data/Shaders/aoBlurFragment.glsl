#version 450

layout(location = 0) in vec2 inUV;
layout(set = 0, binding = 0) uniform sampler2D aoTexture;
layout(location = 0) out vec4 outColor;

void main()
{
    vec2 texelSize = 1.0 / vec2(textureSize(aoTexture, 0));
    vec2 halfTexel = 0.5 * texelSize;

    vec3 sampleX = clamp(inUV.x + vec3(-1.2, 0.0, 1.2) * texelSize.x,
                         halfTexel.x, 1.0 - halfTexel.x);
    vec3 sampleY = clamp(inUV.y + vec3(-1.2, 0.0, 1.2) * texelSize.y,
                         halfTexel.y, 1.0 - halfTexel.y);

    float top = (textureLod(aoTexture, vec2(sampleX.x, sampleY.x), 0.0).r
               + textureLod(aoTexture, vec2(sampleX.z, sampleY.x), 0.0).r) * 0.3125
               + textureLod(aoTexture, vec2(sampleX.y, sampleY.x), 0.0).r * 0.375;
    float middle = (textureLod(aoTexture, vec2(sampleX.x, sampleY.y), 0.0).r
                  + textureLod(aoTexture, vec2(sampleX.z, sampleY.y), 0.0).r) * 0.3125
                  + textureLod(aoTexture, vec2(sampleX.y, sampleY.y), 0.0).r * 0.375;
    float bottom = (textureLod(aoTexture, vec2(sampleX.x, sampleY.z), 0.0).r
                  + textureLod(aoTexture, vec2(sampleX.z, sampleY.z), 0.0).r) * 0.3125
                  + textureLod(aoTexture, vec2(sampleX.y, sampleY.z), 0.0).r * 0.375;

    float visibility = (top + bottom) * 0.3125 + middle * 0.375;
    outColor = vec4(vec3(visibility), 1.0);
}
