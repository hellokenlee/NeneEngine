/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef UTILS_H
#define UTILS_H

#include "Types.h"

//
//    Utils: Static Class to Manage Windows
//

class Utils {
public:
	// 初始化一个窗口
	static void Init(const NNChar* name, NNUInt width, NNUInt height);
	// 终止和清理
	static void Terminate();
	// 轮询IO
	static void PollEvents();
	// 返回是否关闭窗口
	static bool WindowShouldClose();
	static void SetWindowShouldClose(bool flag);
	// 刷新缓冲
	static void Clear();
	// 
	static void Update();
	// 颜色缓冲颜色指定
	static void ClearColor(NNFloat r, NNFloat g, NNFloat b);
	// 交换缓冲
	static void SwapBuffers();
	// 获取窗口大小
	static NNUInt GetWindowWidth();
	static NNUInt GetWindowHeight();
	// 获取当前时间
	static NNFloat GetTime();
	// 获取窗口实例
	static NNWindow GetWindow();
public:
#if defined NENE_DX
	// 获取设备： 用于创建资源
	static ID3D11Device* getDevice();
	// 获取设备上下文： 用于修改资源
	static ID3D11DeviceContext* getContext();
	// 
	static NNUInt mRTVCount;
#endif
private:
	// 窗口大小
	static NNUInt mWinWidth, mWinHeight;
	static NNFloat mBGColor[4];
#if defined NENE_GL
	// 窗口指针
	static GLFWwindow* mpWindow;
#elif defined NENE_DX
	// 初始化D3D11
	static BOOL d3dInit();
	// 窗口句柄
	static HWND mhWindow;
	// 时间消息
	static MSG mMsg;
	// 窗口是否关闭标识
	static BOOL mIsWinShouldClose;
	// 渲染设备
	static ID3D11Device* mpDevice;
	// 设备上下文
	static ID3D11DeviceContext* mpContext;
	// 后缓冲交换链
	static IDXGISwapChain* mpSwapChain;
	// D3D 渲染视图
	static ID3D11RenderTargetView* mpRTV;
	// D3D 深度模板视图
	static ID3D11DepthStencilView* mpDSV;
	// 深度/模板缓冲
	static ID3D11Texture2D* mpDSBuffer;
#else
	#error Please define NENE_GL or NENE_DX in the compiler to specify the GAPI you want to Use.
#endif
};

#endif // UTILS_H