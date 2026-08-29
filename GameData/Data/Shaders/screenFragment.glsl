#version 450

layout(location = 0) in vec2 inUV;

layout(set = 0, binding = 0) uniform sampler2D tex;

layout(location = 0) out vec4 outColor;

void main()
{
    //vec4 col = texture(tex, inUV);
    //outColor = col.xxxw;
    outColor = texture(tex, inUV);
}