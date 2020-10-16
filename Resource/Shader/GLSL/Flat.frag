#version 420 core

layout (binding = 0) uniform sampler2D tex0;

in flat vec3 color_VS_out;

out vec4 color_FS_out;

void main() 
{
	color_FS_out = vec4(color_VS_out, 1.0);
}