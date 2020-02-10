#version 420 core

layout (location = 0) in vec3 position_VS_in;
layout (location = 1) in vec3 normal_VS_in;
layout (location = 2) in vec2 texcoord_VS_in;

layout (std140, binding = 0) uniform UBO0 {
	mat4 view;
	mat4 proj;
	vec3 cam_pos;
	float curr_time, sin_time, cos_time;
	mat4 lightspace;
};

layout (std140, binding = 1) uniform UBO1 {
	mat4 model;
};

out vec2 texcoord_VS_out;
out vec4 lightspace_position_VS_out;

void main() {
	gl_Position = proj * view * model * vec4(position_VS_in, 1.0);
	lightspace_position_VS_out = lightspace * model * vec4(position_VS_in, 1.0);
	texcoord_VS_out = texcoord_VS_in;
}