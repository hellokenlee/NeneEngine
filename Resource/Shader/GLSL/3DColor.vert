#version 420 core

layout (location = 0) in vec3 position_VS_in;
layout (location = 1) in vec3 normal_VS_in;
layout (location = 2) in vec2 uv_VS_in;


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


void main()
{
	gl_Position = proj * view * model * vec4(position_VS_in, 1.0);
}
