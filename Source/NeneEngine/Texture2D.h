/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include <vector>
#include "Texture.h"

//
//    Texture2D: Single Image Texture Class
//

class Texture2D : public Texture {
public:
	//
	~Texture2D();

	// Create texture with one image path
	static std::shared_ptr<Texture2D> Create(const NNChar* filePath);
	// Create texture with multi image paths as mipmaps
	static std::shared_ptr<Texture2D> Create(std::vector<const NNChar*> filepaths);
	// 
	static std::shared_ptr<Texture2D> CreateFromMemory(const NNUInt& width, const NNUInt& height,
		const NNUInt& iformat, const NNUInt& format, const NNUInt& type, const void *pInitData = nullptr);
	//
	static std::shared_ptr<Texture2D> CreateMultisample(const NNUInt& width, const NNUInt& height, const NNUInt& samples, const NNUInt& iformat);
	
	// 
	virtual void Use(const NNUInt& slot = 0);
	// 
	virtual void SetMode(NNTextureMode m);
protected:
	NNTextureMode mMode;
#if defined NENE_GL
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
	Texture2D(const Texture2D& rhs) = delete;
	Texture2D& operator=(const Texture2D& rhs) = delete;

friend class RenderTarget;
};

#endif // TEXTURE2D_H
