#version 420 core

layout (binding = 0) uniform sampler2D tex0;

in vec2 uv_VS_out;

out vec4 color_FS_out;

layout (std140, binding = 3) uniform UBO3
{
	vec3 up;
	vec4 color;
};

void main() 
{
	color_FS_out = vec4(color);
}