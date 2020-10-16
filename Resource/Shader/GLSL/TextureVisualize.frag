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

layout (binding = 0) uniform sampler3D tex0;

layout (std140, binding = 0) uniform UBO0 {
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

in vec2 texcoord_VS_out;

out vec4 color_FS_out;


void main() {
	vec2 texcoord = texcoord_VS_out * texcoord_scale;
	color_FS_out = texture(tex0, vec3(texcoord, lights[0].color.r));
}