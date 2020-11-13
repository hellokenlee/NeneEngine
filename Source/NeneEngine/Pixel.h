/*Copyright reserved by KenLee@2020 hellokenlee@163.com*/
#ifndef PIXEL_H
#define PIXEL_H

#include "Pixel_GL_P.inl"

enum NNPixelFormat
{
	D24S8_UNORM = PixelFormatEnum(UNSIGNED_INT_24_8, DEPTH_STENCIL, DEPTH24_STENCIL8),

	R8G8B8A8_UNORM = PixelFormatEnum(UNSIGNED_BYTE, RGBA, RGBA8),

	R32G32_FLOAT = PixelFormatEnum(FLOAT, RG, RG32F),
	R32G32B32_FLOAT = PixelFormatEnum(FLOAT, RGB, RGB32F),
};

#include "Pixel_GL_S.inl"

#endif // PIXEL_H
