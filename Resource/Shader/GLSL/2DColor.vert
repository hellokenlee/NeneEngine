#version 420 core

layout (location = 0) in vec3 position_VS_in;
layout (location = 1) in vec3 normal_VS_in;
layout (location = 2) in vec2 uv_VS_in;

layout (binding = 0) uniform sampler2D tex0;

out vec2 uv_VS_out;

#define PREVIEW_SCALE 0.5

void main()
{
	vec2 uv = uv_VS_in * 2.0 - vec2(1.0, 1.0);
	vec2 texsize = textureSize(tex0, 0);
	vec2 screensize = vec2(1600, 900);
	vec2 scale = texsize / screensize;
	vec2 offset = vec2(1.0, 1.0) - (texsize / screensize * 0.5);
	vec2 position = (uv * scale * PREVIEW_SCALE) + offset;
	gl_Position = vec4(position, 0.0, 1.0f);
	uv_VS_out = uv_VS_in;
}
