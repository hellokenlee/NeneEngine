#version 420 core

layout (location = 0) in vec3 Position_VS_in;
layout (location = 1) in vec3 Color_VS_in;

out vec3 Color_VS_Out;

layout (std140, binding = 0) uniform UBO0 
{
	mat4 view;
	mat4 proj;
	vec3 camPos;
};

layout (std140, binding = 1) uniform UBO1 
{
	mat4 model;
};

void main() 
{
	vec3 Position = normalize(Position_VS_in);
	gl_Position = proj * view * model * vec4(Position, 1.0);
	Color_VS_Out = Color_VS_in;
}