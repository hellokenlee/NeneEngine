/*Copyright reserved by KenLee@2020 hellokenlee@163.com*/
#if defined NENE_GL
#ifndef PIXEL_GL_P_INL
#define PIXEL_GL_P_INL

#include "Types.h"
#include "Debug.h"

// NNPixelFormat: 32 bits = 8 bits (GL Type) + 8 bits (GL format) + 16 bits (GL InternalFormat)

#define PixelFormatEnum(ptype, pformat, iformat) MakePixelFormatEnumInternal(GL_##ptype, GL_##pformat, GL_##iformat)	

constexpr GLenum GLPixelFormatTypes[] = {
	GL_UNSIGNED_BYTE,
	GL_BYTE,
	GL_UNSIGNED_SHORT,
	GL_SHORT,
	GL_UNSIGNED_INT,
	GL_INT,
	GL_HALF_FLOAT,
	GL_FLOAT,
	GL_UNSIGNED_SHORT_5_6_5,
	GL_UNSIGNED_SHORT_4_4_4_4,
	GL_UNSIGNED_SHORT_5_5_5_1,
	GL_UNSIGNED_INT_2_10_10_10_REV,
	GL_UNSIGNED_INT_10F_11F_11F_REV,
	GL_UNSIGNED_INT_5_9_9_9_REV,
	GL_UNSIGNED_INT_24_8,
	GL_FLOAT_32_UNSIGNED_INT_24_8_REV,
};

constexpr GLenum GLPixelFormatFormats[] = {
	GL_RED,
	GL_RED_INTEGER,
	GL_RG,
	GL_RG_INTEGER,
	GL_RGB,
	GL_RGB_INTEGER,
	GL_RGBA,
	GL_RGBA_INTEGER,
	GL_DEPTH_COMPONENT,
	GL_DEPTH_STENCIL,
	GL_LUMINANCE_ALPHA,
	GL_LUMINANCE,
	GL_ALPHA,
};

constexpr GLenum GLPixelFormatInternalFormats[] = {
	GL_RGB,
	GL_RGBA,
	GL_LUMINANCE_ALPHA,
	GL_LUMINANCE,
	GL_ALPHA,

	GL_R8,
	GL_R8_SNORM,
	GL_R16F,
	GL_R32F,
	GL_R8UI,
	GL_R8I,
	GL_R16UI,
	GL_R16I,
	GL_R32UI,
	GL_R32I,
	GL_RG8,
	GL_RG8_SNORM,
	GL_RG16F,
	GL_RG32F,
	GL_RG8UI,
	GL_RG8I,
	GL_RG16UI,
	GL_RG16I,
	GL_RG32UI,
	GL_RG32I,
	GL_RGB8,
	GL_SRGB8,
	GL_RGB565,
	GL_RGB8_SNORM,
	GL_R11F_G11F_B10F,
	GL_RGB9_E5,
	GL_RGB16F,
	GL_RGB32F,
	GL_RGB8UI,
	GL_RGB8I,
	GL_RGB16UI,
	GL_RGB16I,
	GL_RGB32UI,
	GL_RGB32I,
	GL_RGBA8,
	GL_SRGB8_ALPHA8,
	GL_RGBA8_SNORM,
	GL_RGB5_A1,
	GL_RGBA4,
	GL_RGB10_A2,
	GL_RGBA16F,
	GL_RGBA32F,
	GL_RGBA8UI,
	GL_RGBA8I,
	GL_RGB10_A2UI,
	GL_RGBA16UI,
	GL_RGBA16I,
	GL_RGBA32I,
	GL_RGBA32UI,

	GL_DEPTH_COMPONENT16,
	GL_DEPTH_COMPONENT24,
	GL_DEPTH_COMPONENT32F,
	GL_DEPTH24_STENCIL8,
	GL_DEPTH32F_STENCIL8,
};

constexpr NNUInt MakePixelFormatEnumInternal(GLenum ptype, GLenum pformat, GLenum iformat)
{
	NNUInt type_index = 0;
	for (int i = 0; i < sizeof(GLPixelFormatTypes) / sizeof(GLPixelFormatTypes[0]); ++i)
	{
		type_index = GLPixelFormatTypes[i] == ptype ? i : type_index;
	}
	// static_assert(type_index != -1);

	NNUInt format_index = 0;
	for (int i = 0; i < sizeof(GLPixelFormatFormats) / sizeof(GLPixelFormatFormats[0]); ++i)
	{
		format_index = GLPixelFormatFormats[i] == pformat ? i : format_index;
	}
	// static_assert(format_index != -1);

	NNUInt internal_format_index = 0;
	for (int i = 0; i < sizeof(GLPixelFormatInternalFormats) / sizeof(GLPixelFormatInternalFormats[0]); ++i)
	{
		internal_format_index = GLPixelFormatInternalFormats[i] == iformat ? i : internal_format_index;
	}
	// static_assert(internal_format_index != -1);

	return (type_index << 24) | (format_index << 16) | (internal_format_index << 0);
}

#endif // PIXEL_GL_P_INL
#endif // NENE_GL
