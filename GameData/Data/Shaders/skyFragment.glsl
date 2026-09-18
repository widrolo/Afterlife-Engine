#version 450

layout(location = 4) in vec3 inFragPos;

layout(location = 0) out vec4 outColor;

#include "Lighting.glsl"

layout(set = 0, binding = 0, std140) uniform WorldLightingBlock
{
	WorldLighting lighting;
} world;

const float radius = 400.0;
const vec3 skyColor = vec3(0.53, 0.81, 0.92);
const vec3 voidColorDay = vec3(0.90, 0.90, 0.90);
const vec3 voidColorNight = vec3(0.008, 0.051, 0.188);

#define PI 3.1415926538

vec3 CalcTimedSkyColor(float fac)
{
	return skyColor * fac;
}

vec3 CalcTimedVoidColor(float fac)
{
	return mix(voidColorNight, voidColorDay, fac);
}

// dither cause 24bit color aint even good enough nowadays....
float CalcOrderedDither(vec2 pixel)
{
	const float pattern[64] = float[64](
		 0.0, 32.0,  8.0, 40.0,  2.0, 34.0, 10.0, 42.0,
		48.0, 16.0, 56.0, 24.0, 50.0, 18.0, 58.0, 26.0,
		12.0, 44.0,  4.0, 36.0, 14.0, 46.0,  6.0, 38.0,
		60.0, 28.0, 52.0, 20.0, 62.0, 30.0, 54.0, 22.0,
		 3.0, 35.0, 11.0, 43.0,  1.0, 33.0,  9.0, 41.0,
		51.0, 19.0, 59.0, 27.0, 49.0, 17.0, 57.0, 25.0,
		15.0, 47.0,  7.0, 39.0, 13.0, 45.0,  5.0, 37.0,
		63.0, 31.0, 55.0, 23.0, 61.0, 29.0, 53.0, 21.0);

	int x = int(mod(pixel.x, 8.0));
	int y = int(mod(pixel.y, 8.0));

	return pattern[y * 8 + x] / 64.0 - 0.5;
}

void main()
{
	float yPos = inFragPos.y;

	float timeFac = world.lighting.dayFactor;

	vec3 realSky = CalcTimedSkyColor(timeFac);
	vec3 realVoid = CalcTimedVoidColor(timeFac);


	float t = clamp(yPos / radius * 0.5 + 0.5, 0.0, 1.0);
	vec3 col = mix(realVoid, realSky, smoothstep(0.0, 1.0, t));

	col += CalcOrderedDither(gl_FragCoord.xy) / 255.0;

	outColor = vec4(col, 1.0);
}