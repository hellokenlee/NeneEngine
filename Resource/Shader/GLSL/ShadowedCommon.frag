#version 420 core

#define BIAS 0.001

layout (binding = 0) uniform sampler2D TexDiffuse;
layout (binding = 1) uniform sampler2D TexShadow;

in vec2 texcoord_VS_out;
in vec4 lightspace_position_VS_out;

out vec4 color_FS_out;

float CalcShadowFactor()
{
	return 0.0;
}

void main() 
{
	//
	vec3 lspos = lightspace_position_VS_out.xyz / lightspace_position_VS_out.w;
	lspos = (lspos * 0.5) + 0.5;
	//
	float depth = texture(TexShadow, lspos.xy).r;
	//
	//
	if(depth > 0.0 && lspos.z <= 1.0)
	{
		if(lspos.z > depth)
		{
			color_FS_out = vec4(0.0, 1.0, 0.0, 1.0); // Shadowed
		}
		else
		{
			color_FS_out = vec4(1.0, 0.0, 0.0, 1.0); // Normal
		}
	}
	else
	{
		color_FS_out = vec4(0.9, 0.9, 0.9, 1.0);
	}
	//color_FS_out = texture(TexDiffuse, texcoord_VS_out);
}
