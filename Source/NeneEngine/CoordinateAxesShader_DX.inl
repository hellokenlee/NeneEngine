/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#if defined NENE_DX
#ifndef COORDINATE_AXES_SHADER_DX_INL
#define COORDINATE_AXES_SHADER_DX_INL

// 顶点着色器
const char vShaderSource[] =
	"cbuffer PerFrameCB : register(b0)\n"
	"{\n"
	"	float4x4 view;\n"
	"	float4x4 proj;\n"
	"	float3 camPos;\n"
	"	float _padding;\n"
	"}\n"
	"struct VS_OUT {\n"
	"	float4 position : SV_POSITION;\n"
	"	float3 color : NORMAL;\n"
	"};\n"
	"VS_OUT VS(float3 position : POSITION, float3 color : NORMAL) {\n"
	"	VS_OUT res;\n"
	"	\n"
	"	res.position = float4(position, 1.0);\n"
	"	res.position = mul(res.position, view);\n"
	"	res.position = mul(res.position, proj);\n"
	"	\n"
	"	res.color = color;\n"
	"	return res;\n"
	"}";

// 片元着色器
const char fShaderSource[] =
	"struct VS_OUT {\n"
	"	float4 position : SV_POSITION;\n"
	"	float3 color : normal;\n"
	"};\n"
	"\n"
	"float4 PS(VS_OUT vs_out) : SV_TARGET {\n"
	"	return float4(vs_out.color, 1.0);\n"
	"}\n";

#endif // COORDINATE_AXES_SHADER_DX_INL
#endif // NENE_DX