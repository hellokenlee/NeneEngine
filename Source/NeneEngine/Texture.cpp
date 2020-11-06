/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#include "Debug.h"
#include "Texture.h"
#include <FreeImage/FreeImage.h>

using namespace std;


void Texture::SaveImage(std::shared_ptr<NNByte[]> bits, const NNUInt& width, const NNUInt& height, const NNColorFormat &format, const NNChar* filepath)
{
	int bpp = 32;
	int pitch = ((((bpp * width) + 31) / 32) * 4);
	FIBITMAP *p_image = FreeImage_ConvertFromRawBits(bits.get(), width, height, pitch, bpp, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
	if (p_image != nullptr)
	{
		FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(filepath);
		if (format != FIF_UNKNOWN)
		{
			FreeImage_Save(format, p_image, filepath);
		}
		FreeImage_Unload(p_image);
	}
}

shared_ptr<NNByte[]> Texture::LoadImage(const NNChar* filepath, NNUInt& width, NNUInt& height, NNColorFormat &format)
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
	FREE_IMAGE_COLOR_TYPE color = FreeImage_GetColorType(pImage);
	// !TODO: 避免转换
	if (color != FIC_RGBALPHA) 
	{
		FIBITMAP *tmp = pImage;
		pImage = FreeImage_ConvertTo32Bits(pImage);
		FreeImage_Unload(tmp);
		color = FreeImage_GetColorType(pImage);
	}
	format = RGBA;
	// 获取数据
	pBits = FreeImage_GetBits(pImage);
	width = FreeImage_GetWidth(pImage);
	height = FreeImage_GetHeight(pImage);
	NNUInt pitch = FreeImage_GetPitch(pImage);
	// 拷贝一份数据返回
	NNUInt BBP = FreeImage_GetBPP(pImage) / 8;
	BYTE *res = new BYTE[width * height * BBP];
	memcpy(res, pBits, width * height * BBP);
	// 释放原来的指针
	FreeImage_Unload(pImage);
	// 返回
	return std::unique_ptr<BYTE[]>(res);
}
