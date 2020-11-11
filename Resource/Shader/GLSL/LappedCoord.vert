#version 420 core

layout (location = 0) in vec3 position_VS_in; 	// (SrcTexCoordU, SrcTexCoordV, FaceIndex)
layout (location = 1) in vec2 uv_VS_in; 		// (PatchTexCoordU, PatchTexCoordV)

out vec2 uv_VS_out;

void main()
{
	//
	vec2 pos = position_VS_in.xy;
	pos = pos * 2.0 - vec2(1.0, 1.0);
	gl_Position = vec4(pos, 0.0, 1.0f);
	//
	uv_VS_out = uv_VS_in;
}
