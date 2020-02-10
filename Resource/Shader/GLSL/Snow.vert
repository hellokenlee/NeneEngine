#version 420 core

layout (location = 0) in vec3 position_VS_in;
layout (location = 1) in vec3 normal_VS_in;
layout (location = 2) in vec2 uv_VS_in;

out vec2 uv_VS_out;

void main() {
	gl_Position = vec4(position_VS_in, 1.0f);
	uv_VS_out = uv_VS_in;
}
