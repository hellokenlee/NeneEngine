/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef MOUSE_H
#define MOUSE_H

#include "Utils.h"
#include "Observable.h"

//
//    Mouse: A single to manage mouse device
//

class Mouse {
public:
	// Methods //
	// 获取单例
	static Mouse& Instance();
	// 析构函数
	~Mouse();
	// 更新函数
	void Update();
	// 获取鼠标按键状态
	NNKeyState GetButton(const NNUInt& btncode);
	// 获取指针位置
	NNVec3 GetMousePos();
	// 设置显隐鼠标
	void SetCursorShow(const bool& show);
	//
	inline Observable& OnMove() { return m_on_move; }
	inline Observable& OnClick() { return m_on_click; }
	inline Observable& OnScroll() { return m_on_scroll; }
protected:
	// Varibles //
	Observable m_on_move;
	Observable m_on_click;
	Observable m_on_scroll;
private:
	// Methods //
	Mouse();
	Mouse(const Mouse&);
	Mouse& operator = (const Mouse&);
private:
	// Member Variables //
	NNByte mButtonStates[2][4];
	NNVec3 mCursorPos[2];
	NNUInt mCurrIndex;
#if defined NENE_GL
	// GLFW回调函数
	static void GLFWCursorCallBack(GLFWwindow* window, double nowX, double nowY);
	static void GLFWClickCallBack(GLFWwindow* window, int button, int action, int mods);
	static void GLFWScrollCallBack(GLFWwindow* window, double xoffset, double yoffset);
#elif defined NENE_DX
	// DI 指针
	LPDIRECTINPUT8 mpDirectInput;
	// 设备指针
	IDirectInputDevice8* mpMouse;
	// 鼠标状态
	DIMOUSESTATE mMouseState[2];
	//

#else
	#error Please define NENE_GL or NENE_DX in the compiler to specify the GAPI you want to Use.
#endif
};

#endif // MOUSE_H