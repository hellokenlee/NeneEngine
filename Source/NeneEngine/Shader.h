/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef SHADER_H
#define SHADER_H

#include "Utils.h"
#include <string>
#include <memory>
#include <vector>

//
//    Shader: Compiled GPU program
//

class Shader : public std::enable_shared_from_this<Shader> {
public:
	// 创建着色器
	static std::shared_ptr<Shader> Create(const NNChar *vsFilePath, const NNChar *fsFilePath, 
		const NNVertexFormat lf, const bool& isLink = true);
	static std::shared_ptr<Shader> createFromSource(const NNChar *vsSource, const NNChar *fsSource, 
		const NNVertexFormat lf, const bool& isLink = true);
	static std::shared_ptr<Shader> createFromSource(std::string vsSource, std::string fsSource,
		const NNVertexFormat lf, const bool& isLink = true) 
	{ return createFromSource(vsSource.c_str(), fsSource.c_str(), lf, isLink); }
	// 析构函数
	~Shader();
	// 增加可选着色器
	bool addOptionalShader(const NNChar *filePath, const NNShaderType st, const bool& isLink = true);
	bool addOptionalShaderFromSource(const NNChar *source, const NNShaderType st, const bool& isLink = true);
	// 设置输出类型
	bool setLayoutFormat(const NNVertexFormat vf);
	// 使用该着色器
	void Use();
	// 输出某个着色器的编译信息
	bool checkCompileInfo(const NNShaderType st);
	// 输出整个着色器程序的链接信息
	bool checkLinkInfo();
	
private:
	// 禁止拷贝
	Shader();
	Shader(const Shader& rhs);
	Shader& operator=(const Shader& rhs);
	// 输入格式
	NNVertexFormat mVF;
	//
	std::vector<std::string> mFilepaths;
	// 各个着色器
#if defined NENE_GL
	GLuint mProgramID;
	GLuint mVS, mTCS, mTES, mGS, mFS;
#elif defined NENE_DX
	ID3D11VertexShader* mpVS;
	ID3D11HullShader* mpHS;
	ID3D11ComputeShader* mpCS;
	ID3D11GeometryShader* mpGS;
	ID3D11PixelShader* mpPS;
	ID3DBlob *mpVSError, *mpHSError, *mpCSError, *mpGSError, *mpPSError;
	ID3DBlob *mpVSBlob;
	ID3D11InputLayout* mpLayout;
#else
	#error Please define NENE_GL or NENE_DX in the compiler to specify the GAPI you want to Use.
#endif

};

#endif // SHADER_H