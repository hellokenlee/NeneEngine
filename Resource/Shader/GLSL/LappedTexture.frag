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

out vec4 color_FS_out;

layout (binding = 0) uniform sampler3D tex_hacth;
layout (binding = 1) uniform sampler2D tex_patch;
layout (binding = 2) uniform sampler2D tex_lapped_coord;

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
	// Light Calc
	vec3 light_dir = normalize(lights[0].position.xyz - position_VS_out);
	float diffuse = clamp(dot(light_dir, normal_VS_out), 0.0, 1.0) * lights[0].color.r;
	vec3 view_dir = normalize(camera_position - position_VS_out);
	vec3 halfway_dir = normalize(light_dir + view_dir);
	float specular = pow(max(dot(normal_VS_out, halfway_dir), 0.0), 2.0) * lights[0].color.r;
	float tone = clamp(diffuse, 0.0, 1.0);
	
	//
	vec4 lappedcoord = textureLod(tex_lapped_coord, texcoord_VS_out, 0);
	
	// Sample Hatching Volume Texture
	vec2 texcoord = lappedcoord.xy * texcoord_scale;
	vec3 voltexcoord = vec3(texcoord, 1.0 - tone);
	vec4 final_color = textureLod(tex_hacth, voltexcoord, 0);
	//vec4 final_color = texture(tex_hacth, voltexcoord);


	// final_color.a = lappedcoord.b;


	//
	color_FS_out = final_color;

	// color_FS_out = vec4(lappedcoord.b, 0.0, 0.0, 1.0);

	//float level = textureQueryLod(tex_hacth, voltexcoord).y;
	//color_FS_out = vec4(level, level, level, 1.0);
}