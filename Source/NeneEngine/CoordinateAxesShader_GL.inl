/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#if defined NENE_GL
#ifndef COORDINATE_AXES_SHADER_GL_INL
#define COORDINATE_AXES_SHADER_GL_INL

// 顶点着色器
const char vShaderSource[] =
	"#version 420 core\n"
	"layout (location = 0) in vec3 position_VS_in;\n"
	"layout (location = 1) in vec3 color_VS_in;\n"
	"out vec3 color_VS_out;\n"
	"layout(std140, binding = 0) uniform UBO0 {\n"
	"	mat4 view;\n"
	"	mat4 proj;\n"
	"	vec3 camPos;\n"
	"};\n"
	"void main(){\n"
	"	gl_Position = proj * view * vec4(position_VS_in, 1.0);\n"
	"	color_VS_out = color_VS_in;\n"
	"}\n\0";

// 片元着色器
const char fShaderSource[] =
	"#version 420 core\n"
	"in vec3 color_VS_out;\n"
	"out vec4 color_FS_out;\n"
	"void main(){\n"
	"	color_FS_out = vec4(color_VS_out, 1.0f);\n"
	"}\n\0";

#endif // COORDINATE_AXES_SHADER_GL_INL
#endif // NENE_GL