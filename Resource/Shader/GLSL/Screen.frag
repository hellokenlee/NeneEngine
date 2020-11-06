#version 420 core

layout (binding = 0) uniform sampler2D tex0;

in vec2 uv_VS_out;

out vec4 color_FS_out;


void main()
{
	// color_FS_out = texture(tex0, uv_VS_out);
	// color_FS_out = vec4(1.0, 1.0, 0.0, 1.0);
	color_FS_out = vec4(uv_VS_out.xy, 0.0, 1.0);
}