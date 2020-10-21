#version 420 core

layout (binding = 0) uniform sampler2D tex0;

in vec2 texcoord_VS_out;

out vec4 color_FS_out;

void main() 
{
	color_FS_out = vec4(texture(tex0, texcoord_VS_out).rgb, 1.0);
}
