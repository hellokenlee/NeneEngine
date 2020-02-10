/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "Utils.h"
#include "Observable.h"

//
//    Keyboard: A singleton to manage keyboard device
//

class Keyboard {
public:
	
	// 获取单例
	static Keyboard& Instance();
	// 析构函数
	~Keyboard();
	// 更新函数
	void Update();
	// 获取某按键状态
	NNKeyState GetKey(const NNUInt& keycode);
	//
	inline Observable& OnPress() { return m_on_press; };
	inline Observable& OnRepeat() { return m_on_repeat; };
	inline Observable& OnRelease() { return m_on_release; };
protected:
	// 可监听事件
	Observable m_on_press;
	Observable m_on_repeat;
	Observable m_on_release;
private:
	// 禁止拷贝
	Keyboard();
	Keyboard(const Keyboard&);
	Keyboard& operator = (const Keyboard&);
	// 近两帧的按键状态
	NNByte mKeyStates[2][512];
	// 当前帧的下标
	NNUInt mCurrIndex;
#if defined NENE_GL
	// 回调函数
	static void GLFWKeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mode);
#elif defined NENE_DX
	// DI 指针
	LPDIRECTINPUT8 mpDirectInput;
	// 键盘设备指针
	IDirectInputDevice8* mpKeyboard;
#else
	#error Please define NENE_GL or NENE_DX in the compiler to specify the GAPI you want to Use.
#endif
};

#endif // KEYBOARD_H