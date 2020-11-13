#version 420 core

layout (binding = 0) uniform sampler2D tex_patch;

in vec2 uv_VS_out;

out vec4 color_FS_out;

void main()
{
	//
	vec4 patch_color = texture(tex_patch, uv_VS_out);
	//
	if(patch_color.a < 0.2)
	{
		discard;
	}
	color_FS_out = vec4(uv_VS_out.xy, 0.0, 1.0);
}