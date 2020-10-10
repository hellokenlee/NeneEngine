/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include "Texture.h"

//
//    Texture2D: Single Image Texture Class
//

class Texture2D : public Texture {
public:
	// 创造一个2D纹理
	static std::shared_ptr<Texture2D> Create(const NNChar* filePath);
	static std::shared_ptr<Texture2D> createFromMemory(const NNUInt& width, const NNUInt& height,
		const NNUInt& iformat, const NNUInt& format, const NNUInt& type, const void *pInitData = nullptr);
	static std::shared_ptr<Texture2D> createMultisample(const NNUInt& width, const NNUInt& height, const NNUInt& samples, const NNUInt& iformat);
	// 析构函数
	~Texture2D();
	// 使用该纹理
	virtual void Use(const NNUInt& slot = 0);
	// 更改纹理模式
	virtual void mode(NNTextureMode m);
protected:
	NNTextureMode mMode;
#if defined NENE_GL
	// 纹理编号
	GLuint mTextureID;
#elif defined NENE_DX
	//
	ID3D11Texture2D* mpTexture;
	// 着色器资源
	ID3D11ShaderResourceView *mpSRV;
	// 采样器指针
	ID3D11SamplerState* mpSampler;
#endif
protected:
	Texture2D();
	Texture2D(const Texture2D& rhs);
	Texture2D& operator=(const Texture2D& rhs);

friend class RenderTarget;
};

#endif // TEXTURE2D_H
