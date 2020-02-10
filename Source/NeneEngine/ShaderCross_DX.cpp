/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_DX

#include "IO.h"
#include "Debug.h"
#include "ShaderCross.h"

using namespace std;

string ShaderCross::snapFolder = "";
const ShaderCrossSettings ShaderCross::settings = ShaderCrossSettings();

ShaderCrossSettings::ShaderCrossSettings() {
	//
	STAGE[VERTEX_SHADER] = ShaderConductor::ShaderStage::VertexShader;
	STAGE[FRAGMENT_SHADER] = ShaderConductor::ShaderStage::PixelShader;
	STAGE[GEOMETRY_SHADER] = ShaderConductor::ShaderStage::GeometryShader;
	STAGE[TESS_CONTROL_SHADER] = ShaderConductor::ShaderStage::HullShader;
	STAGE[TESS_EVALUATE_SHADER] = ShaderConductor::ShaderStage::DomainShader;
	//
	HLSL_ENTRY[VERTEX_SHADER] = "VS";
	HLSL_ENTRY[FRAGMENT_SHADER] = "PS";
	HLSL_ENTRY[GEOMETRY_SHADER] = "GS";
	HLSL_ENTRY[TESS_CONTROL_SHADER] = "TCS";
	HLSL_ENTRY[TESS_EVALUATE_SHADER] = "TES";
	//
	GLSL_FILE_EXTENSIONS[VERTEX_SHADER] = ".vert";
	GLSL_FILE_EXTENSIONS[FRAGMENT_SHADER] = ".frag";
	GLSL_FILE_EXTENSIONS[GEOMETRY_SHADER] = ".geom";
	GLSL_FILE_EXTENSIONS[TESS_CONTROL_SHADER] = ".tesc";
	GLSL_FILE_EXTENSIONS[TESS_EVALUATE_SHADER] = ".tese";
	//
	HLSL_FILE_EXTENSIONS[VERTEX_SHADER] = ".vs";
	HLSL_FILE_EXTENSIONS[FRAGMENT_SHADER] = ".ps";
	HLSL_FILE_EXTENSIONS[GEOMETRY_SHADER] = ".gs";
	HLSL_FILE_EXTENSIONS[TESS_CONTROL_SHADER] = ".hs";
	HLSL_FILE_EXTENSIONS[TESS_EVALUATE_SHADER] = ".ds";
}

string ShaderCross::fromHLSL(const NNChar* filepath, NNShaderType stype, const bool snap) {
	return IO::ReadFile(filepath);
}

string ShaderCross::fromHLSLSource(const NNChar* source, NNShaderType stype) {
	return string(source);
}

string ShaderCross::fromGLSL(const NNChar* filepath, NNShaderType stype, const bool snap) {
	//
	string source = IO::ReadFile(filepath);
	dLog("Crossing glsl shader from: %s", filepath);
	//
	string result = fromGLSLSource(source.c_str(), stype);
	//
	if (snap && result != "") {
		string snapFilepath(filepath);
		size_t dot_pos = snapFilepath.find('.');
		snapFilepath = snapFilepath.substr(0, dot_pos);
		snapFilepath += "_DX";
		snapFilepath += settings.HLSL_FILE_EXTENSIONS[stype];
		IO::SaveFile(snapFilepath.c_str(), result.c_str());
		//
		dLog("Snapped cross compile file to: %s", snapFilepath.c_str());
	}
	return result;
}

string ShaderCross::fromGLSLSource(const NNChar* source, NNShaderType stype) {
	return "";
}

#endif // NENE_DX