/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_GL

#include "NeneCB.h"
#include "Shader.h"
#include "IO.h"
#include "Debug.h"
#include <string>
#include <cmath>
#include <map>

using namespace std;

static GLenum s_gl_shader_type[NNShaderType::NNShaderTypeNum] = {
	GL_VERTEX_SHADER,				// VERTEX_SHADER
	GL_FRAGMENT_SHADER,				// FRAGMENT_SHADER
	GL_GEOMETRY_SHADER,				// GEOMETRY_SHADER
	GL_TESS_CONTROL_SHADER,			// TESS_CONTROL_SHADER
	GL_TESS_EVALUATION_SHADER		// TESS_EVALUATE_SHADER
};

static const char* s_gl_shader_name[NNShaderType::NNShaderTypeNum] = {
	"Vertex Shader",				// VERTEX_SHADER
	"Fragment Shader",				// FRAGMENT_SHADER
	"Geometry Shader",				// GEOMETRY_SHADER
	"Tess Control Shader",			// TESS_CONTROL_SHADER
	"Tess Evaluate Shader"			// TESS_EVALUATE_SHADER
};

/** GL Implementation >>> */

class ShaderImpl
{
public:
	//
	ShaderImpl();
	~ShaderImpl();
	//
	void Use();
	//
	bool CheckLinkInfo();
	//
	bool CheckComplieInfo(const NNShaderType st);
public:
	GLint m_is_linked;
	GLuint m_program_id;
	array<GLuint, NNShaderType::NNShaderTypeNum> m_shader_stage_id;
};

ShaderImpl::ShaderImpl(): m_is_linked(false), m_program_id(0), m_shader_stage_id() {}

ShaderImpl::~ShaderImpl()
{
	if (m_program_id != 0)
	{
		glDeleteProgram(m_program_id);
		m_program_id = 0;
	}
}

void ShaderImpl::Use()
{
	if (m_is_linked)
	{
		glUseProgram(m_program_id);
	}
}

bool ShaderImpl::CheckLinkInfo()
{
	//
	glGetProgramiv(m_program_id, GL_LINK_STATUS, &m_is_linked);
	if (!m_is_linked)
	{
		GLchar infoLog[512];
		glGetProgramInfoLog(m_program_id, 512, nullptr, infoLog);
		dLog("[Error]: Link Program Error:\n%s\n\n", infoLog);
	}
	return m_is_linked;
}

bool ShaderImpl::CheckComplieInfo(const NNShaderType st)
{
	//
	if (st >= NNShaderType::NNShaderTypeNum)
	{
		dLog("[Error]: Unknown Shader Type(%d)", st);
		return false;
	}
	//
	GLuint sid = m_shader_stage_id[st]; 
	const char* sname = s_gl_shader_name[st];
	//
	GLint is_succeed = false;
	glGetShaderiv(sid, GL_COMPILE_STATUS, &is_succeed);
	if (!is_succeed)
	{
		static GLchar info_log[1024];
		dCall(glGetShaderInfoLog(sid, 1024, nullptr, info_log));
		dCall(dLog("[Error]: Can not comlile %s:\n%s\n\n", sname, info_log));
	}
	return is_succeed;
}

/** GL Implementation <<< */

Shader::~Shader()
{
	if (m_impl != nullptr)
	{
		delete m_impl;
	}
}

shared_ptr<Shader> Shader::Create(const NNChar *vs_filepath, const NNChar *fs_filepath, const NNVertexFormat vf, const bool try_link)
{
	//
	string vs_source = IO::ReadFile(vs_filepath);
	string fs_source = IO::ReadFile(fs_filepath);
	//
	shared_ptr<Shader> result = CreateFromSource(vs_source.c_str(), fs_source.c_str(), vf, try_link);
	//
	if (result != nullptr)
	{
		result->m_filepaths[NNShaderType::VERTEX_SHADER] = string(vs_filepath);
		result->m_filepaths[NNShaderType::FRAGMENT_SHADER] = string(fs_filepath);
	}
	//
	return result;
}

shared_ptr<Shader> Shader::CreateFromSource(const NNChar *vs_source, const NNChar *fs_source, const NNVertexFormat vf, const bool try_link)
{
	//
	GLuint pid = glCreateProgram();
	if (!pid)
	{
		dLog("[Error] Unable to create shader program!");
		return nullptr;
	}
	//
	Shader* result = new Shader();
	result->m_impl = new ShaderImpl();
	result->m_impl->m_program_id = pid;
	// Compile
	result->AddOptionalShaderFromSource(vs_source, VERTEX_SHADER, false);
	result->AddOptionalShaderFromSource(fs_source, FRAGMENT_SHADER, false);
	// Link and check
	if (try_link)
	{
		glLinkProgram(result->m_impl->m_program_id);
		dCall(result->CheckLinkInfo());
	}
	//
	result->SetLayoutFormat(vf);
	// 
	return shared_ptr<Shader>(result);
}

bool Shader::AddOptionalShader(const NNChar *filepath, const NNShaderType st, const bool& try_link)
{
	string source = IO::ReadFile(filepath);
	m_filepaths[st] = string(filepath);
	return AddOptionalShaderFromSource(source.c_str(), st, try_link);
}

bool Shader::AddOptionalShaderFromSource(const NNChar *source, const NNShaderType st, const bool& try_link)
{
	//
	if (st >= NNShaderType::NNShaderTypeNum)
	{
		dLog("[Error]: Unknown Shader Type(%d)", st);
		return false;
	}
	//
	GLuint& sid = m_impl->m_shader_stage_id[st];
	sid = glCreateShader(s_gl_shader_type[st]);
	//
	bool is_succeed = false;
	//
	glShaderSource(sid, 1, &source, nullptr);
	//
	glCompileShader(sid);
	//
	dCall(is_succeed = CheckCompileInfo(st));
	if (!is_succeed)
	{
		glDeleteShader(sid);
		return false;
	}
	// 
	glAttachShader(m_impl->m_program_id, sid);
	// 
	if (try_link)
	{
		//
		glLinkProgram(m_impl->m_program_id);
		//
		dCall(is_succeed = CheckLinkInfo());
	}
	return is_succeed;
}

void Shader::Use()
{
	m_impl->Use();
}

bool Shader::Recompile()
{
	//
	for (NNUInt st = 0; st < NNShaderType::NNShaderTypeNum; ++st)
	{
		if (m_filepaths[st].size() > 0)
		{
			if (m_impl->m_shader_stage_id[st] != 0)
			{
				glDetachShader(m_impl->m_program_id, m_impl->m_shader_stage_id[st]);
			}
			AddOptionalShader(m_filepaths[st].c_str(), (NNShaderType)st, false);
		}
	}
	//
	glLinkProgram(m_impl->m_program_id);
	//
	return CheckLinkInfo();
}

bool Shader::CheckLinkInfo()
{
	return m_impl->CheckLinkInfo();
}

bool Shader::CheckCompileInfo(const NNShaderType st)
{
	return m_impl->CheckComplieInfo(st);
}

#endif