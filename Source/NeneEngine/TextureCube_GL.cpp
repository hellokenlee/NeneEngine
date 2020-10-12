/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_GL

#include "TextureCube.h"
#include "Texture2D.h"

enum CubeMapBias {
	BIAS_RIGHT = 0,
	BIAS_LEFT,
	BIAS_TOP,
	BIAS_BOTTOM,
	BIAS_BACK,
	BIAS_FRONT,
	CubeMapBiasNum
};

using namespace std;

TextureCube::TextureCube() : Texture(), mTextureID(0) {}

TextureCube::~TextureCube() {
}

shared_ptr<TextureCube> TextureCube::Create(
	const NNChar* filepathTop, const NNChar* filepathBottom,
	const NNChar* filepathLeft, const NNChar* filepathRight,
	const NNChar* filepathFront, const NNChar* filepathBack
) {
	//
	NNColorFormat format;
	GLuint imageWidth = 0, imageHeight = 0;
	std::shared_ptr<NNByte[]> imageData[CubeMapBiasNum];
	//
	imageData[BIAS_RIGHT] = LoadImage(filepathRight, imageWidth, imageHeight, format);
	imageData[BIAS_LEFT] = LoadImage(filepathLeft, imageWidth, imageHeight, format);
	imageData[BIAS_TOP] = LoadImage(filepathTop, imageWidth, imageHeight, format);
	imageData[BIAS_BOTTOM] = LoadImage(filepathBottom, imageWidth, imageHeight, format);
	imageData[BIAS_BACK] = LoadImage(filepathBack, imageWidth, imageHeight, format);
	imageData[BIAS_FRONT] = LoadImage(filepathFront, imageWidth, imageHeight, format);
	//
	GLuint texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
	for (unsigned int i = 0; i < CubeMapBiasNum; ++i) {
		if (imageData[i] != nullptr) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, imageData[i].get());
		}
		else {
			glDeleteTextures(1, &texID);
			return nullptr;
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	//
	TextureCube* ret = new TextureCube();
	ret->mTextureID = texID;
	return shared_ptr<TextureCube>(ret);
}

void TextureCube::Use(const NNUInt& slot) {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureID);
}

#endif // NENE_GL