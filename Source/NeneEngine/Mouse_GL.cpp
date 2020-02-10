/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_GL

#include "Mouse.h"

using namespace std;

Mouse& Mouse::Instance() {
	static Mouse instance;
	return instance;
}

Mouse::Mouse() {
	// 初始化
	memset(mButtonStates[0], 0, 4);
	memset(mButtonStates[1], 0, 4);
	// 设置回调函数
	glfwSetScrollCallback(Utils::GetWindow(), GLFWScrollCallBack);
	glfwSetCursorPosCallback(Utils::GetWindow(), GLFWCursorCallBack);
	glfwSetMouseButtonCallback(Utils::GetWindow(), GLFWClickCallBack);
	// 设置鼠标样式
	glfwSetInputMode(Utils::GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

Mouse::~Mouse() {
	// 恢复
	glfwSetScrollCallback(Utils::GetWindow(), nullptr);
	glfwSetCursorPosCallback(Utils::GetWindow(), nullptr);
	glfwSetMouseButtonCallback(Utils::GetWindow(), nullptr);
}

void Mouse::SetCursorShow(const bool& show) {
	if (show) {
		glfwSetInputMode(Utils::GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	} else {
		glfwSetInputMode(Utils::GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
}

NNKeyState Mouse::GetButton(const NNUInt& btncode) {
	return (NNKeyState)mButtonStates[0][btncode];
}

NNVec3 Mouse::GetMousePos() {
	return mCursorPos[0];
}

void Mouse::Update() {
	;
}

void Mouse::GLFWCursorCallBack(GLFWwindow* window, double nowX, double nowY) {
	// 构造事件
	static shared_ptr<MouseMoveEvent> m_event(new MouseMoveEvent(0.0, 0.0));
	// 事件消息
	m_event->mdx = (NNFloat)(nowX);
	m_event->mdy = (NNFloat)(nowY);
	// 记录位置
	Mouse::Instance().mCursorPos[0].x = (NNFloat)nowX;
	Mouse::Instance().mCursorPos[0].y = (NNFloat)nowY;
	// 分发事件
	Mouse::Instance().OnMove().Notify(m_event);
}

void Mouse::GLFWClickCallBack(GLFWwindow* window, int button, int action, int mods) {
	// 构造事件
	static shared_ptr<KeyboardEvent> k_event(new KeyboardEvent(0, 0));
	// 判断类型
	if (action == GLFW_PRESS) {
		Mouse::Instance().mButtonStates[0][button] = PRESSED;
		k_event->mCode = ON_KEY_PRESS;
	}
	else if (action == GLFW_RELEASE) {
		Mouse::Instance().mButtonStates[0][button] = RELEASED;
		k_event->mCode = ON_KEY_RELEASE;
	}
	else {
		return;
	}
	// 写入消息
	k_event->mKey = button;
	// 发布消息
	Mouse::Instance().OnClick().Notify(k_event);
}

void Mouse::GLFWScrollCallBack(GLFWwindow* window, double xoffset, double yoffset) {
	// 构造事件
	static shared_ptr<MouseScrollEvent> s_event(new MouseScrollEvent(0.0f, 0.0f));
	// 写入消息
	s_event->mdx = (NNFloat)(xoffset);
	s_event->mdy = (NNFloat)(yoffset);
	// 发布消息
	Mouse::Instance().OnScroll().Notify(s_event);
}

#endif