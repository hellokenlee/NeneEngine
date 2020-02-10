/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_DX

#include "Debug.h"
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
	mpKeyboard = nullptr;
	mpDirectInput = nullptr;
	// 获取程序窗口
	HINSTANCE hInstance = GetModuleHandle(0);
	HWND window = Utils::GetWindow();
	// 初始化 DI
	HRESULT hr1 = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)(&mpDirectInput), nullptr);
	dLogIf(FAILED(hr1), "[Error] Failed to init Direct Input!\n");
	// 初始化键盘设备
	HRESULT hr2 = mpDirectInput->CreateDevice(GUID_SysKeyboard, &mpKeyboard, nullptr);
	dLogIf(FAILED(hr2), "[Error] Failed to init keyboard device!\n");
	if (SUCCEEDED(hr2)) {
		// 设置键盘格式
		mpKeyboard->SetDataFormat(&c_dfDIKeyboard);
		mpKeyboard->SetCooperativeLevel(window, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	}
}

Keyboard::~Keyboard() {
	if (mpKeyboard) mpKeyboard->Release();
	if (mpDirectInput) mpDirectInput->Release();
}

void Keyboard::Update() {
	// Ping-Pong Swap
	static NNUInt prevIndex;
	prevIndex = mCurrIndex;
	mCurrIndex ^= 1;
	// 获取状态
	mpKeyboard->Acquire();
	mpKeyboard->GetDeviceState(256, (LPVOID)(&mKeyStates[mCurrIndex]));
	// 检查状态变化
	//!TODO： 把for循环改为DI的回调
	for (int i = 0; i < 256; ++i) {
		if (mKeyStates[mCurrIndex][i] != mKeyStates[prevIndex][i]) {
			// 构造事件
			static Event e(UNDEFINE, new int, sizeof(int));
			// 事件类别
			if (mKeyStates[mCurrIndex][i] & 0x80) {
				e.mCode = ON_KEY_PRESS;
			} else {
				e.mCode = ON_KEY_RELEASE;
			}
			// 写入按键
			*static_pointer_cast<int>(e.mContent) = i;
			// 通知观察者
			OnPress.Notify(e);
		}
	}
	
}

NNKeyState Keyboard::GetKey(const NNUInt& keycode) {
	return (mKeyStates[mCurrIndex][keycode] & 0x80 ? PRESSED : RELEASED);
}


#endif // NENE_GL