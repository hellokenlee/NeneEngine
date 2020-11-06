#version 420 core

layout (location = 0) in vec3 position_VS_in;

out vec2 uv_VS_out;
out float faceid_VS_out;

void main()
{
	vec2 uv = position_VS_in.xy;
	uv = uv * 2.0 - vec2(1.0, 1.0);
	gl_Position = vec4(uv, 0.0, 1.0f);
	uv_VS_out = position_VS_in.xy;
	faceid_VS_out = position_VS_in.z;
}
