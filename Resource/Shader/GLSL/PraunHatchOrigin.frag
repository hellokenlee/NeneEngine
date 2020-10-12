#version 420 core
#define LIGHT_NUM 1

struct Light {
	float type;
	float range;
	vec4 color;
	vec4 position;
	vec4 direction;
	float attenuation;
};


layout (binding = 0) uniform sampler2D tex0;

layout (std140, binding = 0) uniform UBO0 {
	mat4 view;
	mat4 proj;
	vec3 camera_position;
	float curr_time, sin_time, cos_time;
	float _pad0;
	float texcoord_scale;
};

layout (std140, binding = 2) uniform UBO2 {
	Light lights[LIGHT_NUM];
};


in vec2 texcoord_VS_out;
in vec3 normal_VS_out;
in vec3 position_VS_out;

out vec4 color_FS_out;

void main() {
	//
	//vec2 texcoord = texcoord_VS_out * texcoord_scale;
	//color_FS_out = texture(tex0, texcoord);
	float diffuse = dot(normalize(lights[0].position.xyz - position_VS_out.xyz), normal_VS_out);
	color_FS_out = vec4(diffuse, diffuse, diffuse, 1.0);
}