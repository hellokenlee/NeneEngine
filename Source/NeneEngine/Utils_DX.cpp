/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_DX

#include "Utils.h"
#include "Debug.h"
#include "Mouse.h"
#include "NeneCB.h"
#include "Keyboard.h"

#include <string>
#include <ctime>

using namespace std;

// 静态成员初始化
HWND Utils::mhWindow = nullptr;
MSG Utils::mMsg;
BOOL Utils::mIsWinShouldClose = true;
ID3D11Device* Utils::mpDevice = nullptr;
ID3D11DeviceContext* Utils::mpContext = nullptr;
IDXGISwapChain* Utils::mpSwapChain = nullptr;
ID3D11RenderTargetView* Utils::mpRTV = nullptr;
ID3D11DepthStencilView* Utils::mpDSV = nullptr;
ID3D11Texture2D* Utils::mpDSBuffer = nullptr;
NNUInt Utils::mWinWidth = 0, Utils::mWinHeight = 0;
NNUInt Utils::mRTVCount = 1;
NNFloat Utils::mBGColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};

// 显示信息
void ShowEnviroment() {
	// 获取显卡信息
	wstring rendererName(L"Unknown");
	IDXGIAdapter* pAdapter;
	IDXGIFactory* pFactory = nullptr;
	// 创建 DXGI 工厂对象
	if (SUCCEEDED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory))) {
		// 获取第一个显示适配器
		if (pFactory->EnumAdapters(0, &pAdapter) != DXGI_ERROR_NOT_FOUND) {
			DXGI_ADAPTER_DESC adapterDescription;
			pAdapter->GetDesc(&adapterDescription);
			rendererName = adapterDescription.Description;
		}
		// 清理
		if (pFactory) pFactory->Release();
	}
	// 获取 Direct3D 版本
	D3D_FEATURE_LEVEL d3dVersion = Utils::getDevice()->GetFeatureLevel();
	UINT major = (d3dVersion & 0xF000) >> 12;
	UINT minor = (d3dVersion & 0x0F00) >> 8;
	UINT detailMajor = (d3dVersion & 0x00F0) >> 4;
	UINT detailMinor = (d3dVersion & 0x000F) >> 0;
	//!FIXME: 没找到好的方法获取 HLSL 信息
	UINT smMajor = 5, smMinor = 0;
	// 输出信息
	printf("[Info]\n");
	printf("  Working at: \n");
	printf("    %ws\n", rendererName.c_str());
	printf("  Using:\n");
	printf("    Direct3D %d.%d.%d.%d\n", major, minor, detailMajor, detailMinor);
	printf("    HLSL with Shader Model %d.%d\n\n", smMajor, smMinor);
}

// 默认窗口回调
LRESULT CALLBACK NeneWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_DESTROY: {
			PostQuitMessage(0);
			return 0;
		}
		case WM_CLOSE: {
			Utils::setWindowShouldClose(true);
			break;
		}		
		default: {
			break;
		}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// 创建窗口
HWND CreateWindow(const NNChar* name, const NNUInt width, const NNUInt height) {
	//
	HINSTANCE hInstance = GetModuleHandle(0);
	// 默认窗口类名
	static LPCSTR defaultCalssName = "NeneWindowClass";
	// 窗口类 初始化
	WNDCLASSEX winClass = {};
	ZeroMemory(&winClass, sizeof(WNDCLASSEX));
	winClass.cbSize = sizeof(WNDCLASSEX);
	winClass.style = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc = neneWindowProc;
	winClass.hInstance = hInstance;
	winClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	winClass.lpszClassName = defaultCalssName;
	// 注册该窗口类
	RegisterClassEx(&winClass);
	// 获取屏幕分辨率
	RECT screen;
	const HWND desktop = GetDesktopWindow();
	GetWindowRect(desktop, &screen);
	// 计算屏幕正中间位置
	RECT winRect = { 0, 0, (LONG)width, (LONG)height };
	AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, FALSE);
	const int winWidth = winRect.right - winRect.left;
	const int winHeight = winRect.bottom - winRect.top;
	int wndStartPosX = ((screen.right - screen.left) - winWidth) / 2;
	int wndStartPosY = ((screen.bottom - screen.top) - winHeight) / 2;
	// 创建窗口
	HWND res = CreateWindowEx(0, defaultCalssName, name, WS_OVERLAPPEDWINDOW,
		wndStartPosX, wndStartPosY, winWidth, winHeight, nullptr, nullptr,
		hInstance, nullptr);
	//
	return res;
}
// 创建设备和缓冲交换链
BOOL CreateDeviceAndSwapChain(const HWND hOutputWindow, const UINT width, const UINT height,
	IDXGISwapChain* &pSwapChain, ID3D11Device* &pDevice, ID3D11DeviceContext* &pContext) {
	// 初始化缓冲格式描述符
	DXGI_MODE_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Width = width;											   // 后缓冲宽度
	bufferDesc.Height = height;											   // 后缓冲高度
	bufferDesc.RefreshRate.Numerator = 60;                                 // 刷新率分母
	bufferDesc.RefreshRate.Denominator = 1;                                // 刷新率分子  
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;                        // 缓冲格式RGBA共32bits(0-255)
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;    // 扫描线方向(指定从哪个方向开始填充缓冲? )
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;                    // 指定窗口被拉伸后怎么处理图片
	// 初始化一个缓冲交换链描述符
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainDesc.BufferDesc = bufferDesc;                           // 指定上面定义的缓冲
	swapChainDesc.BufferCount = 1;                                   // 后缓冲的数量（使用双缓冲，因此只有一个后缓冲）
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;     // 声明这个交换链用于输出
	swapChainDesc.OutputWindow = hOutputWindow;                      // 绑定输出的窗口句柄
	swapChainDesc.SampleDesc.Count = 4;                              // MSAA采样数
	swapChainDesc.SampleDesc.Quality = 0;                            // 采样质量
	swapChainDesc.Windowed = true;                                   // 窗口显示(而非全屏)
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;    // 允许通过 alt+enter 切换全屏
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;             // 让驱动决定置换缓冲后如何处理旧的缓冲
	// 创建Device和SwapChain !FIXME: 这里处理不了多显示器
	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
		nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, &pDevice, nullptr, &pContext
	);
	dLogIf(FAILED(hr), "[Error] Failed to init render device and swap chain! \n");
	return SUCCEEDED(hr);
}

// 创建渲染视图
BOOL CreateRTV(ID3D11Device* pDevice, IDXGISwapChain* pSwapChain,
	ID3D11RenderTargetView* &pRTV) {
	// 渲染缓冲
	ID3D11Texture2D* pFrameBuffer;
	// 获取后缓冲的地址
	HRESULT hr1 = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pFrameBuffer);
	dLogIf(FAILED(hr1), "[Error] Failed to get frame buffer!");
	// 在后缓冲中创建渲染目标
	HRESULT hr2 = pDevice->CreateRenderTargetView(pFrameBuffer, nullptr, &pRTV);
	dLogIf(FAILED(hr2), "[Error] Failed to Create render target view!");
	// 清理
	pFrameBuffer->Release();
	return (SUCCEEDED(hr1) && SUCCEEDED(hr2));
}

// 创建深度视图
BOOL CreateDSV(ID3D11Device* pDevice, UINT width, UINT height, ID3D11DepthStencilView* &pDSV, 
	ID3D11Texture2D* &pDSBuffer) {
	// 初始化模板和深度缓冲描述符
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 4;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	// 创建模板和深度缓冲
	HRESULT hr1 = pDevice->CreateTexture2D(&depthBufferDesc, nullptr, &pDSBuffer);
	dLogIf(FAILED(hr1), "[Error] Failed to Create depth and stencil buffer!");
	// 创建 DSV
	HRESULT hr2 = pDevice->CreateDepthStencilView(pDSBuffer, nullptr, &pDSV);
	dLogIf(FAILED(hr2), "[Error] Failed to Create depth and stencil view!!");
	return (SUCCEEDED(hr1) && SUCCEEDED(hr2));
}

// 设置视点
void D3dViewport(ID3D11DeviceContext* pContext, FLOAT x, FLOAT y, FLOAT w, FLOAT h) {
	// 初始化视点
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = x;
	viewport.TopLeftY = y;
	viewport.Width = w;
	viewport.Height = h;
	viewport.MaxDepth = 1.0f;
	viewport.MinDepth = 0.0f;
	// 设置视点
	pContext->RSSetViewports(1, &viewport);
}

BOOL Utils::D3dInit() {
	BOOL res = true;
	// 初始化各种东西
	res &= createDeviceAndSwapChain(mhWindow, mWinWidth, mWinHeight, mpSwapChain, mpDevice, mpContext);
	res &= createRTV(mpDevice, mpSwapChain, mpRTV);
	res &= createDSV(mpDevice, mWinWidth, mWinHeight, mpDSV, mpDSBuffer);
	// 设置渲染目标 (1, 渲染视图, 深度视图)
	mpContext->OMSetRenderTargets(1, &mpRTV, mpDSV);
	// 返回状态
	return res;
}

void Utils::Init(const NNChar* name, NNUInt width, NNUInt height) {
	//
	terminate();
	// 提示输出
	printf("[Info] Initializing with Direct3D\n\n");
	// 创建新窗口
	mhWindow = createWindow(name, width, height);
	// 检查创建成功
	if(Utils::mhWindow == nullptr) {
		int errorCode = GetLastError();
		printf("[Error] Failed to Create window(CODE: %d).\n", errorCode);
		exit(-1);
	}
	// 显示窗口
	ShowWindow(Utils::mhWindow, SW_SHOW);
	// 初始化
	mWinHeight = height;
	mWinWidth = width;
	mIsWinShouldClose = false;
	// 初始化 Direct 3D
	if (!d3dInit()) {
		printf("[Error] Failed to init Direct3D 11.\n");
		exit(-1);
	}
	// 设置视点
	d3dViewport(mpContext, 0, 0, (FLOAT)width, (FLOAT)height);
	// 输出信息
	dCall(showEnviroment());
}

void Utils::Terminate() {
	//
	if (mpDSV) mpDSV->Release();
	if (mpRTV) mpRTV->Release();
	if (mpDevice) mpDevice->Release();
	if (mpContext) mpContext->Release();
	if (mpDSBuffer) mpDSBuffer->Release();
	if (mpSwapChain) mpSwapChain->Release();
	//
	mpDSV = nullptr;
	mpRTV = nullptr;
	mpDevice = nullptr;
	mpContext = nullptr;
	mpDSBuffer = nullptr;
	mpSwapChain = nullptr;
}

void Utils::PollEvents() {
	// 处理Windows事件
	if (PeekMessage(&mMsg, nullptr, 0, 0, PM_REMOVE)) {
		// 如果有消息，处理消息
		TranslateMessage(&mMsg);
		DispatchMessage(&mMsg);
		// 处理退出
		if (mMsg.message == WM_QUIT) {
			mIsWinShouldClose = true;
		}
	}
	// 更新输入设备
	Keyboard::Instance().Update();
	Mouse::Instance().Update();
}

void Utils::Update() {
	//
	Utils::pollEvents();
	//
	NNFloat currTime = Utils::getTime();
	NNFloat sinTime = sinf(currTime);
	NNFloat cosTime = cosf(currTime);
	//
	static NeneCB& CB = NeneCB::Instance();
	CB.PerFrame().data.currTime = currTime;
	CB.PerFrame().data.sinTime = sinTime;
	CB.PerFrame().data.cosTime = cosTime;
}

void Utils::SetWindowShouldClose(bool flag) {
	mIsWinShouldClose = flag;
}

bool Utils::WindowShouldClose() {
	return mIsWinShouldClose;
}

void Utils::ClearColor(NNFloat r, NNFloat g, NNFloat b) {
	mBGColor[0] = r;
	mBGColor[1] = g;
	mBGColor[2] = b;
	mBGColor[3] = 1.0f;
}

void Utils::Clear() {
	//
	static vector<ID3D11RenderTargetView*> pRTVs;
	static ID3D11DepthStencilView* pDSV;
	pRTVs.resize(mRTVCount);
	// 获取渲染对象
	mpContext->OMGetRenderTargets(mRTVCount, pRTVs.data(), &pDSV);
	//
	for (NNUInt i = 0; i < mRTVCount; ++i) {
		mpContext->ClearRenderTargetView(pRTVs[i], mBGColor);
	}
	mpContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Utils::SwapBuffers() {
	mpSwapChain->Present(1, 0);
}

NNUInt Utils::GetWindowWidth() {
	return mWinWidth;
}

NNUInt Utils::GetWindowHeight() {
	return mWinHeight;
}

ID3D11Device* Utils::GetDevice() {
	return mpDevice;
}

ID3D11DeviceContext* Utils::GetContext() {
	return mpContext;
}

NNWindow Utils::GetWindow() {
	return mhWindow;
}

NNFloat Utils::GetTime() {
	return (NNFloat)clock() / (NNFloat)CLOCKS_PER_SEC;
}

#endif // NENE_DX