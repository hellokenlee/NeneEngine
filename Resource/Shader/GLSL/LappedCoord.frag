#version 420 core

in vec2 uv_VS_out;

out vec4 color_FS_out;

void main()
{
	//
	color_FS_out = vec4(uv_VS_out.xy, 0.0, 1.0);
}