#version 420 core

layout (binding = 0) uniform sampler2D tex0;
layout (binding = 1) uniform sampler2D tex1;
layout (binding = 2) uniform sampler2D tex2;

layout (std140, binding = 2) uniform UBO1 {
	float factor;
};

#define TRANSITION 0.1

in vec2 uv_VS_out;

out vec4 color_FS_out;

void main() {
	vec4 diff = texture(tex0, uv_VS_out);
	vec4 height = texture(tex1, uv_VS_out);
	vec4 snow = texture(tex2, uv_VS_out);
	color_FS_out = diff;
	if(height.r < factor - TRANSITION)
	{
		color_FS_out = snow;
	}
	else if(height.r < factor)
	{
		float alpha = (height.r - (factor - TRANSITION)) / TRANSITION;
		color_FS_out = mix(diff, snow, alpha);
	}
}