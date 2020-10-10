/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_GL
#include "Debug.h"
#include "Texture2D.h"

using namespace std;

Texture2D::Texture2D() : Texture(), mTextureID(0), mMode(AS_COLOR) {}


Texture2D::~Texture2D() {
	if (mTextureID != 0) {
		glDeleteTextures(1, &mTextureID);
	}
}

shared_ptr<Texture2D> Texture2D::createFromMemory(const NNUInt& width, const NNUInt& height, const NNUInt& iformat, const NNUInt& format, const NNUInt& type, const void *pInitData) {
	//
	GLuint texID = 0;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
		glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)iformat, width, height, 0, (GLenum)format, (GLenum)type, pInitData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	//
	if (texID == 0) {
		return nullptr;
	}
	Texture2D* ret = new Texture2D();
	ret->mTextureID = texID;
	return shared_ptr<Texture2D>(ret);
}

shared_ptr<Texture2D> Texture2D::createMultisample(const NNUInt& width, const NNUInt& height, const NNUInt& samples, const NNUInt& iformat) {
	//
	GLuint texID = 0;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texID);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, (GLenum)iformat, width, height, false);
	glBindTexture(GL_TEXTURE_2D, 0);
	//
	if (texID == 0) {
		return nullptr;
	}
	Texture2D* ret = new Texture2D();
	ret->mTextureID = texID;
	return shared_ptr<Texture2D>(ret);
}


shared_ptr<Texture2D> Texture2D::Create(const NNChar* filepath) {
	// 初始化
	NNUInt texID = 0;
	std::shared_ptr<NNByte[]> imageData = nullptr;
	// 宽，高
	GLuint width, height;
	NNColorFormat format;
	// 读取图片
	imageData = loadImage(filepath, width, height, format);
	if (imageData == nullptr || width == 0 || height == 0) {
		dLog("[Error] Broken image data! Could not load texture(%s)\n", filepath);
		return nullptr;
	}
	// 生产纹理
	glGenTextures(1, &texID);
	// 指定纹理类型
	glBindTexture(GL_TEXTURE_2D, texID);
		// 设置纹理参数
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// 绑定图片数据
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, imageData.get());
	// 恢复状态机状态
	glBindTexture(GL_TEXTURE_2D, 0);
	//
	if (texID == 0) {
		return nullptr;
	}
	Texture2D* ret = new Texture2D();
	ret->mTextureID = texID;
	return shared_ptr<Texture2D>(ret);
}

void Texture2D::Use(const NNUInt& slot) {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
}

void Texture2D::mode(NNTextureMode m) {
	if (m == mMode) {
		return;
	}
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	switch (m)
	{
	case AS_COLOR:
		// 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	case AS_DEPTH:
		// 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
		break;
	case AS_STENCIL:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_DEPTH_COMPONENT);
		break;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

#endif // NENE_GL