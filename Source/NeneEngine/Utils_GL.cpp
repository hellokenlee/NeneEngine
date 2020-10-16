/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_GL

#include "Debug.h"
#include "Utils.h"
#include "NeneCB.h"

// 静态成员初始化
GLFWwindow* Utils::mpWindow = nullptr;
NNUInt Utils::mWinWidth = 0, Utils::mWinHeight = 0;
NNFloat Utils::mBGColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

// 输出相关信息
void showEnviroment() {
	printf("[Info]:\n");
	printf("  Working at:\n");
	printf("    %s\n", glGetString(GL_RENDERER));
	printf("  Using:\n");
	printf("    OpenGL %s\n", glGetString(GL_VERSION));
	printf("    GLSL %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("    GLEW %s\n", glewGetString(GLEW_VERSION));
	printf("    GLFW %s\n\n", glfwGetVersionString());
}

void Utils::Init(const NNChar* name, NNUInt width, NNUInt height) {
	//
	Utils::Terminate();
	// 提示输出
	printf("[Info]: Initializing with OpenGL\n");
	// 初始化GLFW
	if (!glfwInit()) {
		printf("[Error]: Initialization of GLFW Failed!\n");
		exit(-1);
	}
	// MSAA 4
	// glfwWindowHint(GLFW_SAMPLES, 4);
	// 不允许Resize
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	// 创建窗口
	mpWindow = glfwCreateWindow(width, height, name, nullptr, nullptr);
	// 检查是否成功
	if (mpWindow == nullptr) {
		printf("[Error]: Fail to Create GLFW window\n");
		glfwTerminate();
		exit(-1);
	}
	// 记录窗口大小
	mWinHeight = height;
	mWinWidth = width;
	// 获取显示器大小
	const GLFWvidmode *screen;
	screen = glfwGetVideoMode(glfwGetPrimaryMonitor());
	// 设置窗口在中间
	glfwSetWindowPos(mpWindow, 
		(screen->width - width) / 2, (screen->height - height) / 2);
	// 初始化渲染上下文
	glfwMakeContextCurrent(mpWindow);
	//
	glfwSwapInterval(1);
	// 初始化GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		printf("[Error]: Fail to init GLEW\n");
		exit(-1);
	}
	//
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_3D);
	glEnable(GL_TEXTURE_2D);
	glDepthFunc(GL_LEQUAL);
	// 忽略由glew引起的INVALID_ENUM错误
	glGetError();
	//
#if NNDEBUG > 0
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(_dMessageCallback, nullptr);
#endif
	// 设置视点
	glViewport(0, 0, width, height);
	// 输出信息
	dCall(showEnviroment());
}

void Utils::Terminate() {
	if (mpWindow != nullptr) {
		glfwDestroyWindow(mpWindow);
		glfwTerminate();
		mpWindow = nullptr;
	}
	mWinHeight = 0;
	mWinWidth = 0;
}

void Utils::PollEvents() {
	glfwPollEvents();
}

void Utils::Update() {
	//
	Utils::PollEvents();
	//
	NNFloat currTime = Utils::GetTime();
	NNFloat sinTime = sinf(currTime);
	NNFloat cosTime = cosf(currTime);
	//
	static NeneCB& CB = NeneCB::Instance();
	CB.PerFrame().Data().curr_time = currTime;
	CB.PerFrame().Data().sin_time = sinTime;
	CB.PerFrame().Data().cos_time = cosTime;
}

bool Utils::WindowShouldClose() {
	return glfwWindowShouldClose(mpWindow);
}

void Utils::SetWindowShouldClose(bool flag) {
	glfwSetWindowShouldClose(mpWindow, flag);
}

void Utils::ClearColor(NNFloat r, NNFloat g, NNFloat b) {
	mBGColor[0] = r;
	mBGColor[1] = g;
	mBGColor[2] = b;
	mBGColor[3] = 1.0f;
	glClearColor(r, g, b, 1.0f);
}

void Utils::Clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Utils::SwapBuffers() {
	glfwSwapBuffers(mpWindow);
}

NNUInt Utils::GetWindowWidth() {
	return mWinWidth;
}

NNUInt Utils::GetWindowHeight() {
	return mWinHeight;
}

NNWindow Utils::GetWindow() {
	return mpWindow;
}

NNFloat Utils::GetTime() {
	return (NNFloat)glfwGetTime();
}
#endif