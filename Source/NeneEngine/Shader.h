/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef SHADER_H
#define SHADER_H

#include <array>
#include <vector>
#include <string>
#include <memory>

#include "Utils.h"

class ShaderImpl;

//
//    Shader: Compiled GPU program
//

class Shader : public std::enable_shared_from_this<Shader>
{
public:
	// Destruct
	~Shader();
	// Creation
	static std::shared_ptr<Shader> Create(const NNChar *vs_filepath, const NNChar *fs_filepath, 
		const NNVertexFormat vf=POSITION_NORMAL_TEXTURE, const bool try_link=true);
	static std::shared_ptr<Shader> CreateFromSource(const NNChar *vs_source, const NNChar *fs_source, 
		const NNVertexFormat vf=POSITION_NORMAL_TEXTURE, const bool try_link=true);
	
	// Set vertex format
	void SetLayoutFormat(const NNVertexFormat vf) { m_vf = vf; }
	// Add optional shader
	bool AddOptionalShader(const NNChar *filePath, const NNShaderType st, const bool& isLink = true);
	bool AddOptionalShaderFromSource(const NNChar *source, const NNShaderType st, const bool& isLink = true);
	
	// Use this shader for rendering
	void Use();
	//
	bool Recompile();
	// Check the linking informations
	bool CheckLinkInfo();
	// Check the compile informations
	bool CheckCompileInfo(const NNShaderType st);

	static void RecompileAllShaders();

private:
	ShaderImpl* m_impl;
	NNVertexFormat m_vf;
	std::array<std::string, NNShaderType::NNShaderTypeNum> m_filepaths;

private:
	static std::vector<std::weak_ptr<Shader>> s_all_shaders;

private:
	// 
	Shader() = default;
	Shader(const Shader& rhs) = delete;
	Shader& operator=(const Shader& rhs) = delete;
};

#endif // SHADER_H