#version 420 core

layout (binding = 0) uniform sampler2D TexDiffuse;
layout (binding = 1) uniform sampler2D TexShadow;

in vec2 texcoord_VS_out;
in vec4 lightspace_position_VS_out;

out vec4 color_FS_out;

#define PCF_WIDTH 2

float CalcShadow(vec4 lspos)
{
	// 透视除法
	vec3 lsppos = lspos.xyz / lspos.w;
	lsppos = lsppos * 0.5 + 0.5;
	//
	if(lsppos.z > 1.0)
	{
		return 0.0;
	}
	//
	float shadow = 0.0;
	vec2 texsize = 1.0 / textureSize(TexShadow, 0);
	for(int x = -PCF_WIDTH; x <= PCF_WIDTH; ++x)
	{
		for (int y = -PCF_WIDTH; y <= PCF_WIDTH; ++y)
		{
			float depth = texture(TexShadow, lsppos.xy + vec2(x, y) * texsize).r;
			shadow += lsppos.z > depth ? 1.0 : 0.0;
		}
	}
	shadow /= (PCF_WIDTH + 1) * (PCF_WIDTH + 1);
	return shadow;
}

void main() 
{
	vec3 color = texture(TexDiffuse, texcoord_VS_out).rgb;
	float shadow = CalcShadow(lightspace_position_VS_out);
	color = (1.0 - shadow) * color;
	color_FS_out = vec4(color, 1.0);
}
