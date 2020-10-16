/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_GL

#include "NeneCB.h"
#include "Shader.h"
#include "IO.h"
#include "Debug.h"
#include <string>
#include <cmath>

using namespace std;

Shader::Shader() :
	mVF(UNDEFINE),
	mFilepaths(NNShaderTypeNum, ""),
	mProgramID(0),
	mVS(0),
	mGS(0),
	mFS(0),
	mTCS(0),
	mTES(0)
	{}

Shader::~Shader() {
	if (mProgramID != 0) {
		glDeleteProgram(mProgramID);
	}
}

shared_ptr<Shader> Shader::Create(const NNChar *vsFilePath, const NNChar *fsFilePath,
	const NNVertexFormat lf, const bool& isLink) {
	//
	string vsSource = IO::ReadFile(vsFilePath);
	string fsSource = IO::ReadFile(fsFilePath);
	//
	return createFromSource(vsSource.c_str(), fsSource.c_str(), lf, isLink);
}

shared_ptr<Shader> Shader::createFromSource(const NNChar *vsSource, const NNChar *fsSource, 
	const NNVertexFormat lf, const bool& isLink) {
	// 着色器对象
	Shader *res = new Shader();
	res->mProgramID = glCreateProgram();
	// 编译
	res->AddOptionalShaderFromSource(vsSource, VERTEX_SHADER, false);
	res->AddOptionalShaderFromSource(fsSource, FRAGMENT_SHADER, false);
	// 检查
	if (isLink) {
		glLinkProgram(res->mProgramID);
		dCall(res->checkLinkInfo());
	}
	// 创建失败
	if (!res->mVS || !res->mFS || !res->mProgramID) {
		delete res;
		return nullptr;
	}
	// 返回对象
	return shared_ptr<Shader>(res);
}

bool Shader::AddOptionalShader(const NNChar *filePath, const NNShaderType st, const bool& isLink) {
	string source = IO::ReadFile(filePath);
	return AddOptionalShaderFromSource(source.c_str(), st, isLink);
}

bool Shader::AddOptionalShaderFromSource(const NNChar *source, const NNShaderType st, const bool& isLink) {
	//
	GLuint *pID = nullptr;
	switch (st) {
		case VERTEX_SHADER: {
			mVS = glCreateShader(GL_VERTEX_SHADER);
			pID = &mVS;
			break;
		}
		case FRAGMENT_SHADER: {
			mFS = glCreateShader(GL_FRAGMENT_SHADER);
			pID = &mFS;
			break;
		}
		case TESS_CONTROL_SHADER: {
			mTCS =  glCreateShader(GL_TESS_CONTROL_SHADER);
			pID = &mTCS;
			break;
		}
		case TESS_EVALUATE_SHADER: {
			mTES = glCreateShader(GL_TESS_EVALUATION_SHADER);
			pID = &mTES;
			break;
		}
		case GEOMETRY_SHADER: {
			mGS = glCreateShader(GL_GEOMETRY_SHADER);
			pID = &mGS;
			break;
		}
		default: {
			dLog("[Error]: Unknown Shader Type(%d)\n", st);
			return false;
		}
	}
	//
	bool isSuccess = false;
	// 指定着色器代码
	glShaderSource(*pID, 1, &source, nullptr);
	// 编译
	glCompileShader(*pID);
	// 检查编译情况
	dCall(isSuccess = checkCompileInfo(st));
	if (!isSuccess) {
		glDeleteShader(*pID);
		return false;
	}
	// 链接到着色器程序上
	glAttachShader(mProgramID, *pID);
	// 
	if (isLink) {
		glLinkProgram(mProgramID);
		// 检查链接情况
		dCall(isSuccess = checkLinkInfo());
	}
	return isSuccess;
}

void Shader::Use() {
	if (mIsLinked)
	{
		glUseProgram(mProgramID);
	}
}

bool Shader::checkCompileInfo(const NNShaderType st) {
	GLuint sID = 0;
	static string sName = "Unknow Shader";
	switch (st) {
		case VERTEX_SHADER: {
			sID = mVS;
			sName = "Vertex Shader";
			break;
		}
		case TESS_CONTROL_SHADER: {
			sID = mTCS;
			sName = "Tess Control Shader";
			break;
		}
		case TESS_EVALUATE_SHADER: {
			sID = mTES;
			sName = "Tess Evaluate Shader";
			break;
		}
		case GEOMETRY_SHADER: {
			sID = mGS;
			sName = "Geometry Shader";
			break;
		}
		case FRAGMENT_SHADER: {
			sID = mFS;
			sName = "Fragment Shader";
			break;
		}
		default: {
			dLog("[Error]: Unknown Shader Type(%d)", st);
			return false;
		}
	}
	GLint isSuccess;
	glGetShaderiv(sID, GL_COMPILE_STATUS, &isSuccess);
	if (!isSuccess) {
		static GLchar infoLog[512];
		dCall(glGetShaderInfoLog(sID, 512, nullptr, infoLog));
		dCall(dLog("[Error]: Can not comlile %s:\n%s\n\n", sName.c_str(), infoLog));
	}
	return isSuccess;
}

bool Shader::checkLinkInfo() {
	glGetProgramiv(mProgramID, GL_LINK_STATUS, &mIsLinked);
	if (!mIsLinked) {
		GLchar infoLog[512];
		glGetProgramInfoLog(mProgramID, 512, nullptr, infoLog);
		dLog("[Error]: Link Program Error:\n%s\n\n", infoLog);
	}
	return mIsLinked;
}

#endif