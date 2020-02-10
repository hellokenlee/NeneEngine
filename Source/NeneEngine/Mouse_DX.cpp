/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_DX

#include "Debug.h"
#include "Mouse.h"

using namespace std;

//!TODO: Replace direct input.

Mouse& Mouse::Instance() {
	static Mouse instance;
	return instance;
}

Mouse::Mouse() {
	// 获取程序窗口
	HINSTANCE hInstance = GetModuleHandle(0);
	HWND window = Utils::GetWindow();
	// 初始化 DI
	HRESULT hr1 = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)(&mpDirectInput), nullptr);
	dLogIf(FAILED(hr1), "[Error] Failed to init Direct Input!\n");
	// 初始化鼠标设备
	HRESULT hr2 = mpDirectInput->CreateDevice(GUID_SysMouse, &mpMouse, nullptr);
	dLogIf(FAILED(hr2), "[Error] Failed to init mouse device!\n");
	if (SUCCEEDED(hr2)) {
		mpMouse->SetDataFormat(&c_dfDIMouse);
		mpMouse->SetCooperativeLevel(window, DISCL_NOWINKEY | DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	}
	//
	mCursorPos[0] = NNVec3(0.0f, 0.0f, 0.0f);
	mCursorPos[1] = NNVec3(0.0f, 0.0f, 0.0f);
	//
	mCurrIndex = 0;
}

Mouse::~Mouse() {
	if (mpMouse) mpMouse->Release();
	if (mpDirectInput) mpDirectInput->Release();
}

void Mouse::SetCursorShow(const bool& show) {
	;
}

void Mouse::Update() {
	// Ping-pong
	static NNUInt prevIndex;
	prevIndex = mCurrIndex;
	mCurrIndex ^= 1;
	// 从DI获取相对坐标变化
	mpMouse->Acquire();
	mpMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mMouseState[mCurrIndex]);
	// 处理光标移动
	mCursorPos[mCurrIndex].x = (NNFloat)mMouseState[mCurrIndex].lX;
	mCursorPos[mCurrIndex].y = (NNFloat)mMouseState[mCurrIndex].lY;
	mCursorPos[mCurrIndex].z = (NNFloat)mMouseState[mCurrIndex].lZ;
	// 累加坐标
	mCursorPos[mCurrIndex].x += mCursorPos[prevIndex].x;
	mCursorPos[mCurrIndex].y += mCursorPos[prevIndex].y;
	mCursorPos[mCurrIndex].z += mCursorPos[prevIndex].z;
	//
	if(mCursorPos[mCurrIndex].x != mCursorPos[prevIndex].x || mCursorPos[mCurrIndex].y != mCursorPos[prevIndex].y) {
		static Event e(ON_MOUSE_MOVE, new NNFloat[2], sizeof(NNFloat) * 2);
		static_pointer_cast<NNFloat[]>(e.mContent)[0] = mCursorPos[mCurrIndex].x;
		static_pointer_cast<NNFloat[]>(e.mContent)[1] = mCursorPos[mCurrIndex].y;
		onMove.Notify(e);
	}
	// 处理按钮点击
	memcpy(mButtonStates[mCurrIndex], mMouseState[mCurrIndex].rgbButtons, sizeof(BYTE) * 4);
	for (int i = 0; i < 4; ++i) {
		if (mButtonStates[mCurrIndex][i] != mButtonStates[prevIndex][i]) {
			static Event e(UNDEFINE, new int, sizeof(int));
			if (mButtonStates[mCurrIndex][i] & 0x80) {
				e.mCode = ON_KEY_PRESS;
			} else {
				e.mCode = ON_KEY_RELEASE;
			}
			*static_pointer_cast<int>(e.mContent) = NN_MOUSE_BUTTON_LEFT + i;
			onClick.Notify(e);
		}
	}
	
}

NNKeyState Mouse::GetButton(const NNUInt& btnCode) {
	return (mButtonStates[mCurrIndex][btnCode] & 0x80) ? PRESSED : RELEASED;
}

NNVec3 Mouse::GetMousePos() {
	return mCursorPos[mCurrIndex];
}

#endif
