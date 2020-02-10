/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef TEXTURE_3D_H
#define TEXTURE_3D_H

#include "Texture.h"

class Texture3D : public Texture {
public:
	// 创建纹理
	static std::shared_ptr<Texture3D> Create(const NNChar* filePath);
	// 析构函数
	~Texture3D();
	// 使用该纹理
	virtual void Use(const NNUInt& slot = 0);
	// 更改纹理模式
	virtual void mode(NNTextureMode m);
};

#endif // TEXTURE_3D_H
