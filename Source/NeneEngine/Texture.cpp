/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#include "Debug.h"
#include "Texture.h"

#include <FreeImage/FreeImage.h>

using namespace std;


void Texture::SaveImage(std::shared_ptr<NNByte[]> bits, const NNUInt& width, const NNUInt& height, const NNPixelFormat &format, const NNChar* filepath)
{
	FIBITMAP *p_image = nullptr;
	if (format == NNPixelFormat::R32G32B32_FLOAT)
	{
		int bpp = 8 * sizeof(FIRGBF);
		int pitch = ((((bpp * width) + 31) / 32) * 4);
		p_image = FreeImage_ConvertFromRawBitsEx(true, bits.get(), FIT_RGBF, width, height, pitch, bpp, 0, 0, 0);
	}
	else if (format == NNPixelFormat::B8G8R8A8_UNORM)
	{
		int bpp = 32;
		int pitch = ((((bpp * width) + 31) / 32) * 4);
		p_image = FreeImage_ConvertFromRawBits(bits.get(), width, height, pitch, bpp, 0, 0, 0);
	}
	else if (format == NNPixelFormat::R8G8B8A8_UNORM)
	{
		//
		int bpp = 32;
		int pitch = ((((bpp * width) + 31) / 32) * 4);
		// RGBA -> BGRA
		for (NNUInt y = 0; y < height; ++y)
		{
			for (NNUInt x = 0; x < width; ++x)
			{
				NNByte& r = bits[(y * width * (bpp / 8)) + (x * (bpp / 8)) + 0];
				NNByte& b = bits[(y * width * (bpp / 8)) + (x * (bpp / 8)) + 2];
				std::swap(r, b);
			}
		}
		p_image = FreeImage_ConvertFromRawBits(bits.get(), width, height, pitch, bpp, 0, 0, 0);
	}
	if (p_image != nullptr)
	{
		FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(filepath);
		if (format != FIF_UNKNOWN)
		{
			FreeImage_Save(format, p_image, filepath, EXR_FLOAT);
		}
		FreeImage_Unload(p_image);
	}
}

shared_ptr<NNByte[]> Texture::LoadImage(const NNChar* filepath, NNUInt& width, NNUInt& height, NNUInt& bpp, NNPixelFormat& format)
{
	// 检查文件
	checkFileExist(filepath);
	// 图片格式
	FREE_IMAGE_FORMAT fileFormat = FIF_UNKNOWN;
	// 图片数据
	BYTE *pBits = nullptr;
	// 图片指针
	FIBITMAP *pImage = nullptr;
	// 获取格式
	fileFormat = FreeImage_GetFileType(filepath, 0);
	if (fileFormat == FIF_UNKNOWN) 
	{
		fileFormat = FreeImage_GetFIFFromFilename(filepath);
	}
	if (fileFormat == FIF_UNKNOWN)
	{
		dLog("[Error] Unknown image type (%s)\n", filepath);
		return nullptr;
	}
	// 载入图片
	if (FreeImage_FIFSupportsReading(fileFormat))
	{
		pImage = FreeImage_Load(fileFormat, filepath);
	}
	else
	{
		dLog("[Error] Unsupported image type (%s)\n", filepath);
	}
	if (!pImage)
	{
		dLog("[Error] Failed to load image! (%s)\n", filepath);
		return nullptr;
	}
	//
	FREE_IMAGE_COLOR_TYPE fic = FreeImage_GetColorType(pImage);
	//
	FREE_IMAGE_TYPE fit = FreeImage_GetImageType(pImage);
	//
	bpp = FreeImage_GetBPP(pImage);
	//
	format = NNPixelFormat::INVALID;
	//
	if (fit == FIT_BITMAP and fic == FIC_RGB)
	{
		format = NNPixelFormat::B8G8R8_UNORM;
	}
	else if (fit == FIT_BITMAP and fic == FIC_RGBALPHA)
	{
		format = NNPixelFormat::B8G8R8A8_UNORM;
	}
	else if (fit == FIT_RGBF and fic == FIC_RGB)
	{
		format = NNPixelFormat::R32G32B32_FLOAT;
	}
	//
	if (format == NNPixelFormat::INVALID)
	{
		dLog("[Error] Unsupport image format! (%s)\n", filepath);
		return nullptr;
	}
	// 获取数据
	pBits = FreeImage_GetBits(pImage);
	width = FreeImage_GetWidth(pImage);
	height = FreeImage_GetHeight(pImage);
	NNUInt pitch = FreeImage_GetPitch(pImage);
	// 拷贝一份数据返回
	BYTE *res = new BYTE[width * height * (bpp / 8)];
	memcpy(res, pBits, width * height * (bpp / 8));
	// 释放原来的指针
	FreeImage_Unload(pImage);
	// 返回
	return std::unique_ptr<BYTE[]>(res);
}
