#version 420 core

layout (binding = 0) uniform sampler2D tex0;

in vec2 uv_VS_out;
in float faceid_VS_out;

out vec4 color_FS_out;


void main()
{
	// RG encode an uint face id up to 2^16
	//
	uint face = uint(faceid_VS_out);
	//
	uint face_r = (face & 0xFF00) >> 8;
	uint face_g = (face & 0x00FF) >> 0;
	//
	color_FS_out.a = 1.0;
	//
	color_FS_out.r = float(face_r) / 255.0;
	color_FS_out.g = float(face_g) / 255.0;
	//
	color_FS_out.b = texture(tex0, uv_VS_out).a;
}