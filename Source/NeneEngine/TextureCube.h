/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef TEXTURE_CUBE_H
#define TEXTURE_CUBE_H

#include "Texture.h"

//
//    TextureCube: Cube texture with 6 images
//

class TextureCube : public Texture {
public:
	// 创造一个Cube纹理
	static std::shared_ptr<TextureCube> Create(
		const NNChar* filepathTop, const NNChar* filepathBottom,
		const NNChar* filepathLeft, const NNChar* filepathRight,
		const NNChar* filepathFront, const NNChar* filepathBack
	);
	// 析构函数
	~TextureCube();
	// 使用该纹理
	virtual void Use(const NNUInt& slot = 0);
private:
#if defined NENE_GL
	// 纹理编号
	GLuint mTextureID;
#elif defined NENE_DX
	// 着色器资源
	ID3D11ShaderResourceView *mpSRV;
	// 采样器指针
	ID3D11SamplerState* mpSampler;
	//
	ID3D11Texture2D* mpTex;
#endif
private:
	TextureCube();
	TextureCube(const TextureCube& rhs);
	TextureCube& operator=(const TextureCube& rhs);
};

#endif // TEXTURE_CUBE_H
