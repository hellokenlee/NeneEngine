#version 420 core

layout (binding = 0) uniform sampler3D tex0;

layout (std140, binding = 0) uniform UBO0 {
	mat4 view;
	mat4 proj;
	vec3 camera_position;
	float curr_time, sin_time, cos_time;
	float _pad0;
	float texcoord_scale;
};

in vec2 texcoord_VS_out;

out vec4 color_FS_out;


void main() {
	color_FS_out = texture(tex0, vec3(texcoord_VS_out, texcoord_scale));
}