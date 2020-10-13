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


out vec2 texcoord_VS_out;
out vec3 normal_VS_out;
out vec3 position_VS_out;
out vec3 weight012_VS_out;
out vec3 weight345_VS_out;
out float intensity_debug_VS_out;


layout (std140, binding = 0) uniform UBO0 
{
	mat4 view;
	mat4 proj;
	vec3 camera_position;
	float curr_time, sin_time, cos_time;
	float _pad0;
	float texcoord_scale;
};

layout (std140, binding = 1) uniform UBO1 
{
	mat4 model;
};

layout (std140, binding = 2) uniform UBO2 
{
	Light lights[LIGHT_NUM];
};


void main() {
	//
	gl_Position = proj * view * model * vec4(position_VS_in, 1.0);
	texcoord_VS_out = texcoord_VS_in;
	position_VS_out = vec3(model * vec4(position_VS_in, 1.0f));
	normal_VS_out = mat3(transpose(inverse(model))) * normal_VS_in;
	//
	float intensity = clamp(dot(normalize(lights[0].position.xyz - position_VS_out), normal_VS_out), 0.0, 1.0);
	float tone = intensity * 6.0;
	//
	intensity_debug_VS_out = intensity;
	weight012_VS_out = vec3(0.0);
	weight345_VS_out = vec3(0.0);
	//
	if(tone >= 5.0)
	{
		// Blend 0th tone with pure white
		weight012_VS_out.x = 1.0 - intensity;
	}
	else if(tone >= 4.0)
	{
		weight012_VS_out.xy = vec2(intensity - 2.0 / 6.0, 1.0 - (intensity - 2.0 / 6.0));
	}
	else if(tone >= 3.0)
	{
		weight012_VS_out.yz = vec2(intensity - 3.0 / 6.0, 1.0 - (intensity - 3.0 / 6.0));
	}
	else if(tone >= 2.0)
	{
		weight012_VS_out.z = intensity - 4.0 / 6.0;
		weight345_VS_out.x = 1.0 - weight012_VS_out.z;
	}
	else if(tone >= 1.0)
	{
		weight345_VS_out.xy = vec2(intensity - 5.0 / 6.0, 1.0 - (intensity - 5.0 / 6.0) );
	}
	else
	{
		// Blend 5th tone with pure black
		weight345_VS_out.yz = vec2(intensity - 1.0, 1.0 - (intensity - 1.0) );
	}
}