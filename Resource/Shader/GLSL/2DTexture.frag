#version 420 core

layout (binding = 0) uniform sampler2D tex0;

in vec2 uv_VS_out;

out vec4 color_FS_out;

void main() 
{
	color_FS_out = texture(tex0, uv_VS_out);
}