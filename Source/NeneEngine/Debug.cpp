/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/

#include "Debug.h"

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

using namespace std;

// 检查文件存在
void _dCheckFileExist(const char* fileName) {
#ifdef _WIN32
	if (_access(fileName, 0) == -1) {
		printf("[Error] File not exist! (%s)\n\n", fileName);
		exit(-1);
	}
#else
	if (access(fileName, F_OK)) {
		printf("[Error] File not exist! (%s)\n\n", fileName);
		exit(-1);
	}
#endif
}


#if defined NENE_GL
void _dCheckGLError(const char* file, const int& line) {

	//
	static char filepath[128];
	static int linenum;
	static GLenum errcode;
	static GLuint finite;
	static char* filename;
	finite = 16;
	//
	while (errcode = glGetError(), errcode != GL_NO_ERROR && finite--) {
		filename = strrchr(filepath, '\\');
		if (filename != nullptr) {
			++filename;
		}
		ddLog("File: \"%s\", line %d, error %d.\n\n\n", filename, linenum, errcode);
	}
	//
	strcpy(filepath, file);
	linenum = line;
}

void GLAPIENTRY _dMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	
	if(id == 131169 || id == 131185 || id == 131218 || id == 131204) {
		return;
	}

	printf("---------------------\n");
	printf("OpenGL Error: %d\n", id);
	printf("Message: %s\n", message);
	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             printf("Source: API"); break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   printf("Source: Window System"); break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: printf("Source: Shader Compiler"); break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     printf("Source: Third Party"); break;
	case GL_DEBUG_SOURCE_APPLICATION:     printf("Source: Application"); break;
	case GL_DEBUG_SOURCE_OTHER:           printf("Source: Other"); break;
	}
	printf("\n");

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               printf("Type: Error"); break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: printf("Type: Deprecated Behaviour"); break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  printf("Type: Undefined Behaviour"); break;
	case GL_DEBUG_TYPE_PORTABILITY:         printf("Type: Portability"); break;
	case GL_DEBUG_TYPE_PERFORMANCE:         printf("Type: Performance"); break;
	case GL_DEBUG_TYPE_MARKER:              printf("Type: Marker"); break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          printf("Type: Push Group"); break;
	case GL_DEBUG_TYPE_POP_GROUP:           printf("Type: Pop Group"); break;
	case GL_DEBUG_TYPE_OTHER:               printf("Type: Other"); break;
	}
	printf("\n");

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         printf("Severity: High"); break;
	case GL_DEBUG_SEVERITY_MEDIUM:       printf("Severity: Medium"); break;
	case GL_DEBUG_SEVERITY_LOW:          printf("Severity: Low"); break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: printf("Severity: Notification"); break;
	} 
	printf("\n");
}

#endif