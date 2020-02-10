/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_GL

#include "Keyboard.h"

using namespace std;

Keyboard& Keyboard::Instance() {
	static Keyboard instance;
	return instance;
}

Keyboard::Keyboard() {
	// 初始化
	memset(mKeyStates[0], 0, 512);
	memset(mKeyStates[1], 0, 512);
	mCurrIndex = 0;
	//
	glfwSetKeyCallback(Utils::GetWindow(), GLFWKeyCallBack);
}

Keyboard::~Keyboard() {
	glfwSetKeyCallback(Utils::GetWindow(), nullptr);
}

void Keyboard::Update() {
	;
}

NNKeyState Keyboard::GetKey(const NNUInt& keycode) {
	return (NNKeyState)mKeyStates[0][keycode];
}

void Keyboard::GLFWKeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mode) {
	// 构造事件
	static shared_ptr<KeyboardEvent> k_event(new KeyboardEvent(0, 0));
	// 写入按键编号到事件中
	k_event->mKey = key;
	// 分发事件
	switch (action)
	{
	case GLFW_PRESS:
		Keyboard::Instance().mKeyStates[0][key] = PRESSED;
		k_event->mCode = ON_KEY_PRESS;
		Keyboard::Instance().m_on_press.Notify(k_event);
		break;
	case GLFW_REPEAT:
		Keyboard::Instance().mKeyStates[0][key] = PRESSED;
		k_event->mCode = ON_KEY_REPEAT;
		Keyboard::Instance().m_on_repeat.Notify(k_event);
		break;
	case GLFW_RELEASE:
		Keyboard::Instance().mKeyStates[0][key] = RELEASED;
		k_event->mCode = ON_KEY_RELEASE;
		Keyboard::Instance().m_on_release.Notify(k_event);
		break;
	default:
		break;
	}
}

#endif // NENE_GL