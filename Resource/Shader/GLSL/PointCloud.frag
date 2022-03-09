#version 420 core
in vec3 Color_VS_Out;
out vec4 color_FS_out;

void main() 
{
	color_FS_out = vec4(Color_VS_Out, 1.0);
}