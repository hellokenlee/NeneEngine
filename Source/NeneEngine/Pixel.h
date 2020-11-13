/*Copyright reserved by KenLee@2020 hellokenlee@163.com*/
#ifndef PIXEL_H
#define PIXEL_H

#include "Pixel_GL_P.inl"

enum NNPixelFormat
{
	INVALID = PixelFormatEnum(INVALID_VALUE, INVALID_VALUE, INVALID_VALUE),
	
	D24S8_UNORM = PixelFormatEnum(UNSIGNED_INT_24_8, DEPTH_STENCIL, DEPTH24_STENCIL8),

	B8G8R8_UNORM = PixelFormatEnum(UNSIGNED_BYTE, BGR, RGB),
	R8G8B8_UNORM = PixelFormatEnum(UNSIGNED_BYTE, RGB, RGB8),
	B8G8R8A8_UNORM = PixelFormatEnum(UNSIGNED_BYTE, BGRA, RGBA),
	R8G8B8A8_UNORM = PixelFormatEnum(UNSIGNED_BYTE, RGBA, RGBA8),

	R32G32_FLOAT = PixelFormatEnum(FLOAT, RG, RG32F),
	R32G32B32_FLOAT = PixelFormatEnum(FLOAT, RGB, RGB32F),
};

#include "Pixel_GL_S.inl"

#endif // PIXEL_H
