#version 420 core

layout (triangles) in;
layout (line_strip, max_vertices = 2) out;

in vec3 normal_VS_out[];
in vec3 position_VS_out[];

layout (std140, binding = 0) uniform UBO0
{
	mat4 view;
	mat4 proj;
	vec3 camera_pos;
};

layout (std140, binding = 3) uniform UBO3
{
	vec3 up;
};

void EmitTangentVertices()
{
	vec3 a = position_VS_out[0];
	vec3 b = position_VS_out[1];
	vec3 c = position_VS_out[2];
	vec3 cent = (a + b + c) / 3.0;
	//
	vec3 na = normalize(normal_VS_out[0]);
	vec3 nb = normalize(normal_VS_out[1]);
	vec3 nc = normalize(normal_VS_out[2]);
	vec3 normal = normalize((na + nb + nc) / 3.0);
	//
	// vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 tangent = normalize(up - (dot(up, normal) * normal));
	//
	cent = cent + normal * 0.01;
	gl_Position = proj * view * vec4(cent, 1.0);
	EmitVertex();
	gl_Position = proj * view * vec4(cent + (tangent * 0.1), 1.0);
	EmitVertex();
	EndPrimitive();
}


void EmitNormalVertices()
{
	//
	vec3 a = position_VS_out[0];
	vec3 b = position_VS_out[1];
	vec3 c = position_VS_out[2];
	vec3 cent = (a + b + c) / 3.0;
	//
	vec3 na = normalize(normal_VS_out[0]);
	vec3 nb = normalize(normal_VS_out[1]);
	vec3 nc = normalize(normal_VS_out[2]);
	vec3 normal = normalize((na + nb + nc) / 3.0);
	//
	gl_Position = proj * view * vec4(cent, 1.0);
	EmitVertex();
	gl_Position = proj * view * vec4(cent + (normal * 0.1), 1.0);
	EmitVertex();
	EndPrimitive();
}


void main()
{
	EmitTangentVertices();
}
