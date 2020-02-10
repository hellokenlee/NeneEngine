/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_GL

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
}

string ShaderCross::fromHLSL(const NNChar* filepath, NNShaderType stype, const bool snap) {
	//
	string source = IO::ReadFile(filepath);
	dLog("Crossing hlsl shader from: %s", filepath);
	//
	string result = fromHLSLSource(source.c_str(), stype);
	//
	if (snap && result != "") {
		string snapFilepath(filepath);
		size_t dot_pos = snapFilepath.find('.');
		snapFilepath = snapFilepath.substr(0, dot_pos);
		snapFilepath += "_GL";
		snapFilepath += settings.GLSL_FILE_EXTENSIONS[stype];
		IO::SaveFile(snapFilepath.c_str(), result.c_str());
		//
		dLog("Snapped cross compile file to: %s", snapFilepath.c_str());
	}
	return result;
}

string ShaderCross::fromHLSLSource(const NNChar* source, NNShaderType stype) {
	// Init
	ShaderConductor::Compiler::SourceDesc sourceDesc{};
	ShaderConductor::Compiler::TargetDesc targetDesc{};
	ShaderConductor::Compiler::Options options{};
	// Nene Use colum-major
	options.packMatricesInRowMajor = false;
	// Input spec
	sourceDesc.fileName = "NeneShaderGL";
	sourceDesc.stage = settings.STAGE[stype];
	sourceDesc.source = source;
	sourceDesc.entryPoint = settings.HLSL_ENTRY[stype].c_str();
	// Output spec
	targetDesc.version = "420";
	targetDesc.language = ShaderConductor::ShadingLanguage::Glsl;
	// Compile
	string glSource;
	auto result = ShaderConductor::Compiler::Compile(sourceDesc, options, targetDesc);
	// Log errors or warnnings
	if (result.errorWarningMsg != nullptr) {
		const char* msg = reinterpret_cast<const char*>(result.errorWarningMsg->Data());
		dLog("[Info]: Cross shader from HLSL to GLSL:\n%s\n\n", string(msg, msg + result.errorWarningMsg->Size()).c_str());
	}
	// Save the translation
	if (result.target != nullptr) {
		glSource = string(reinterpret_cast<const char*>(result.target->Data()), result.target->Size());
	}
	// Clean up
	ShaderConductor::DestroyBlob(result.errorWarningMsg);
	ShaderConductor::DestroyBlob(result.target);
	//
	return glSource;
}

string ShaderCross::fromGLSL(const NNChar* filepath, NNShaderType stype, const bool snap) {
	return IO::ReadFile(filepath);
}

string ShaderCross::fromGLSLSource(const NNChar* source, NNShaderType stype) {
	return string(source);
}

#endif // NENE_GL