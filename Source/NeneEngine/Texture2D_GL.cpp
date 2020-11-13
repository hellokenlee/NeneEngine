/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_GL
#include <vector>
#include "Debug.h"
#include "Texture2D.h"

using namespace std;

/** Implementation Functions >>> */

void GenerateTextures(vector<GLuint> IN tex_ids)
{

}

/** Implementation Functions <<< */

Texture2D::Texture2D() : Texture(), mTextureID(0), mMode(AS_COLOR) {}


Texture2D::~Texture2D()
{
	if (mTextureID != 0)
	{
		glDeleteTextures(1, &mTextureID);
	}
}

shared_ptr<Texture2D> Texture2D::CreateFromMemory(const NNUInt& width, const NNUInt& height, const NNPixelFormat& format, const void *init_data)
{
	//
	GLuint texID = 0;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
		glTexImage2D(GL_TEXTURE_2D, 0, GetGLInternalFormat(format), width, height, 0, GetGLFormat(format), GetGLType(format), init_data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	//
	if (texID == 0)
	{
		return nullptr;
	}
	Texture2D* ret = new Texture2D();
	ret->mTextureID = texID;
	ret->m_width = width;
	ret->m_height = height;
	ret->m_format = format;
	return shared_ptr<Texture2D>(ret);
}

shared_ptr<Texture2D> Texture2D::CreateMultisample(const NNUInt& width, const NNUInt& height, const NNUInt& samples, const NNUInt& iformat) 
{
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
	ret->m_width = width;
	ret->m_height = height;
	return shared_ptr<Texture2D>(ret);
}


shared_ptr<Texture2D> Texture2D::Create(const NNChar* filepath)
{
	return Create(vector<const NNChar*>({filepath}));
}


shared_ptr<Texture2D> Texture2D::Create(vector<const NNChar*> filepaths)
{
	//
	NNUInt texID = 0;
	glGenTextures(1, &texID);
	//
	if (texID == 0)
	{
		dLog("[Error] Cannot generate texture!\n");
		return nullptr;
	}
	//
	GLuint width = 0, height = 0;
	//
	glBindTexture(GL_TEXTURE_2D, texID);
	{
		//
		for (NNUInt idx = 0; idx < filepaths.size(); ++idx)
		{
			//
			const char* filepath = filepaths[idx];
			shared_ptr<NNByte[]> image_data = nullptr;
			//
			NNUInt bpp;
			NNPixelFormat format;
			//
			image_data = LoadImage(filepath, width, height, bpp, format);
			if (image_data == nullptr || width == 0 || height == 0)
			{
				dLog("[Error] Broken image data! Could not load texture(%s)\n", filepath);
				continue;
			}
			//
			glTexImage2D(GL_TEXTURE_2D, idx, GetGLInternalFormat(format), width, height, 0, GetGLFormat(format), GetGLType(format), image_data.get());
		}
		//
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, (GLint)filepaths.size() - 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -2);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	//
	Texture2D* ret = new Texture2D();
	ret->mTextureID = texID;
	ret->m_width = width;
	ret->m_height = height;
	ret->m_format = NNPixelFormat::R8G8B8A8_UNORM;
	return shared_ptr<Texture2D>(ret);
}

void Texture2D::Use(const NNUInt& slot) 
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
}

void Texture2D::SetSampler(std::shared_ptr<Sampler> sampler)
{
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampler->m_mag_filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampler->m_min_filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sampler->m_wrap_u);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sampler->m_wrap_v);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

shared_ptr<NNByte[]> Texture2D::GetPixelData()
{
	//
	if (mTextureID == 0)
	{
		return nullptr;
	}
	//
	NNUInt pixel_width = 4;
	if (m_format == NNPixelFormat::R32G32B32_FLOAT)
	{
		pixel_width = 96;
	}
	NNByte* buffer = new NNByte[m_width * m_height * pixel_width * sizeof(NNByte)];
	//
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	{
		glGetTexImage(GL_TEXTURE_2D, 0, GetGLFormat(m_format), GetGLType(m_format), buffer);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	//
	return shared_ptr<NNByte[]>(buffer);
}

void Texture2D::SavePixelData(const NNChar* filepath)
{
	shared_ptr<NNByte[]> bits = GetPixelData();
	if (bits != nullptr)
	{
		SaveImage(bits, m_width, m_height, m_format, filepath);
	}
}

#endif // NENE_GL
