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

layout (location = 0) in vec3 position_VS_in;
layout (location = 1) in vec3 normal_VS_in;
layout (location = 2) in vec2 texcoord_VS_in;

out flat vec3 color_VS_out;

layout (std140, binding = 0) uniform UBO0 
{
	mat4 view;
	mat4 proj;
	vec3 camera_pos;
};

layout (std140, binding = 1) uniform UBO1
{
	mat4 model;
};

layout (std140, binding = 2) uniform UBO2 
{
	Light lights[LIGHT_NUM];
};


void main() 
{
	//
	gl_Position = proj * view * model * vec4(position_VS_in, 1.0);
	//
	vec3 normal = normalize(mat3(transpose(inverse(model))) * normal_VS_in);
	vec3 position = vec3(model * vec4(position_VS_in, 1.0f));
	//
	vec3 light_dir = normalize(lights[0].position.xyz - position);
	float diffuse = clamp(dot(light_dir, normal), 0.0, 1.0) * lights[0].color.r;
	color_VS_out = vec3(diffuse);
}