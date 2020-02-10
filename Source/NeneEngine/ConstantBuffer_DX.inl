/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#if defined NENE_DX

#ifndef CONSTANT_BUFFER_DX_INL
#define CONSTANT_BUFFER_DX_INL

#include "Debug.h"

#include <cstring>

template<typename T>
ConstantBuffer<T>::ConstantBuffer() {
	//
	mpBuffer = nullptr;
	// 初始化常量缓冲描述符
	D3D11_BUFFER_DESC CBDesc;
	ZeroMemory(&CBDesc, sizeof(D3D11_BUFFER_DESC));
	CBDesc.Usage = D3D11_USAGE_DEFAULT;
	CBDesc.ByteWidth = sizeof(T);
	CBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	// 创建常量缓冲
	HRESULT hr = Utils::getDevice()->CreateBuffer(&CBDesc, nullptr, &mpBuffer);
	// 检查
	dLogIf(sizeof(T) % 16 != 0, "[Error]: D3D11 require constant buffer size must be multiple of 16 bytes!\n");
	dLogIf(FAILED(hr), "[Error]: Failed to Create constant buffer!\n");
}

template<typename T>
ConstantBuffer<T>::~ConstantBuffer() {
	if (mpBuffer) mpBuffer->Release();
}

template<typename T>
void ConstantBuffer<T>::Update(const UINT& slot) {
	// 更新数据
	Utils::getContext()->UpdateSubresource(mpBuffer, 0, nullptr, &data, 0, 0);
	// 给每个着色器设置缓冲
	Utils::getContext()->VSSetConstantBuffers(slot, 1, &mpBuffer);
	//Utils::getContext()->GSSetConstantBuffers(slot, 1, &mpBuffer);
	//Utils::getContext()->HSSetConstantBuffers(slot, 1, &mpBuffer);
	//Utils::getContext()->CSSetConstantBuffers(slot, 1, &mpBuffer);
	Utils::getContext()->PSSetConstantBuffers(slot, 1, &mpBuffer);
}
#endif // CONSTANT_BUFFER_DX_INL
#endif // NENE_DX