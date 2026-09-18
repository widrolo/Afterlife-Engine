#ifndef LIGHTING_GLSL
#define LIGHTING_GLSL

const int MAX_LIGHTS = 15;

struct SunLightInfo
{
    vec3 direction;
    vec4 color;
    float intensity;
};

struct LightData
{
    vec3 position;
    vec3 direction;
    vec4 color;
    float intensity;
    float range;
    float innerCone;
    float outerCone;
};

struct LightSource
{
    int type;
    LightData data;
};

struct WorldLighting
{
    SunLightInfo sun;
    vec4 ambient;
    float ambIntensity;
    uint numLights;
    LightSource sources[MAX_LIGHTS];
};

#endif
