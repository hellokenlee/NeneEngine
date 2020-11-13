/*Copyright reserved by KenLee@2020 hellokenlee@163.com*/
#if defined NENE_GL
#ifndef PIXEL_GL_S_INL
#define PIXEL_GL_S_INL

inline GLenum GetGLType(const NNPixelFormat& format)
{
	return GLPixelFormatTypes[(format & 0xff000000)>> 24];
}

inline GLenum GetGLFormat(const NNPixelFormat& format)
{
	return GLPixelFormatFormats[(format & 0x00ff0000) >> 16];
}

inline GLenum GetGLInternalFormat(const NNPixelFormat& format)
{
	return GLPixelFormatInternalFormats[(format & 0x0000ffff) >> 00];
}

#endif // PIXEL_GL_S_INL
#endif // NENE_GL
