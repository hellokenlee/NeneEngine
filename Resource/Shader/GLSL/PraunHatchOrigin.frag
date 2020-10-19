#version 420 core
#define LIGHT_NUM 1

struct Light 
{
	float type;
	float range;
	vec4 color;
	vec4 position;
	vec4 direction;
	float attenuation;
};

in vec2 texcoord_VS_out;
in vec3 normal_VS_out;
in vec3 position_VS_out;
in vec3 weight012_VS_out;
in vec3 weight345_VS_out;
in float intensity_debug_VS_out;

out vec4 color_FS_out;

layout (binding = 0) uniform sampler2D tex_hatch_tone012;
layout (binding = 1) uniform sampler2D tex_hatch_tone345;

layout (std140, binding = 0) uniform UBO0 
{
	mat4 view;
	mat4 proj;
	vec3 camera_position;
	float curr_time, sin_time, cos_time;
	float _pad0;
	float texcoord_scale;
};

layout (std140, binding = 2) uniform UBO2 
{
	Light lights[LIGHT_NUM];
};


void main() {
	//
	vec2 texcoord = texcoord_VS_out * texcoord_scale;
	//
	vec4 hatch_color012 = texture(tex_hatch_tone012, texcoord);
	vec4 hatch_color345 = texture(tex_hatch_tone345, texcoord);
	//
	float blend_color012 = dot(weight012_VS_out.xyz, hatch_color012.rgb);
	float blend_color345 = dot(weight345_VS_out.xyz, hatch_color345.rgb);
	//
	float whiteness = 1.0 - dot((weight012_VS_out + weight345_VS_out), vec3(1.0));
	//
	float final_color = blend_color012 + blend_color345 + whiteness;
	color_FS_out = vec4(final_color, final_color, final_color, 1.0);
	// color_FS_out = vec4(intensity_debug_VS_out, intensity_debug_VS_out, intensity_debug_VS_out, 1.0);
	// float level = textureQueryLod(tex_hatch_tone012, texcoord).y;
	// color_FS_out = vec4(level, level, level, 1.0);
}