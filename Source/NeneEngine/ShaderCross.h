/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef SHADER_CROSS_H
#define SHADER_CROSS_H

#include "Utils.h"

#include <ShaderConductor/ShaderConductor.hpp>
#include <string>

//
//    ShaderCross: 
//

class ShaderCrossSettings {
public:
	ShaderCrossSettings();
public:
	std::string HLSL_ENTRY[NNShaderTypeNum];
	std::string GLSL_FILE_EXTENSIONS[NNShaderTypeNum];
	std::string HLSL_FILE_EXTENSIONS[NNShaderTypeNum];
	ShaderConductor::ShaderStage STAGE[NNShaderTypeNum];
};

class ShaderCross {
public:
	static std::string fromHLSL(const NNChar* filepath, NNShaderType stype, const bool snap = true);
	static std::string fromGLSL(const NNChar* filepath, NNShaderType stype, const bool snap = true);
	static std::string fromHLSLSource(const NNChar* source, NNShaderType stype);
	static std::string fromGLSLSource(const NNChar* source, NNShaderType stype);
public:
	static std::string snapFolder;
	const static ShaderCrossSettings settings;
};

#endif // SHADER_CROSS_H