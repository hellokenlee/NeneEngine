/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_DX
#include "IO.h"
#include "Debug.h"
#include "Texture2D.h"

using namespace std;

Texture2D::Texture2D() : mpSRV(nullptr), mpSampler(nullptr), mpTexture(nullptr), Texture(){}

Texture2D::~Texture2D() {
	if (mpSRV) mpSRV->Release();
	if (mpTexture) mpTexture->Release();
	if (mpSampler) mpSampler->Release();
}

shared_ptr<Texture2D> Texture2D::Create(const NNChar* filepath) {
	checkFileExist(filepath);
	//
	HRESULT res = S_OK;
	size_t len = strlen(filepath);
	//
	shared_ptr<NNByte[]> imageData = nullptr;
	NNUInt width, height;
	NNColorFormat format;
	imageData = loadImage(filepath, width, height, format);
	//
	ID3D11Texture2D* pTexture;
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	//
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = imageData.get();
	initData.SysMemPitch = static_cast<UINT>(width * 4);
	initData.SysMemSlicePitch = static_cast<UINT>(width * height * 4);
	//
	res = Utils::getDevice()->CreateTexture2D(&textureDesc, &initData, &pTexture);
	if (FAILED(res)) {
		dLog("[Error] Failed to Create texture 2D!\n");
		return nullptr;
	}
	// 创建着色器视图
	ID3D11ShaderResourceView* pSRV;
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.Format = textureDesc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MostDetailedMip = 0;
	SRVDesc.Texture2D.MipLevels = -1;
	res = Utils::getDevice()->CreateShaderResourceView(pTexture, &SRVDesc, &pSRV);
	if (FAILED(res)) {
		dLog("[Error] Failed to Create SRV!\n");
		pTexture->Release();
		return nullptr;
	}
	// 创建采样器
	ID3D11SamplerState* pSampler;
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	res = Utils::getDevice()->CreateSamplerState(&samplerDesc, &pSampler);
	if (FAILED(res)) {
		dLog("[Error] Failed to Create texture sampler!\n");
		pSRV->Release();
		pTexture->Release();
		return nullptr;
	}
	//
	Texture2D *ret = new Texture2D();
	ret->mpSRV = pSRV;
	ret->mpTexture = pTexture;
	ret->mpSampler = pSampler;
	return shared_ptr<Texture2D>(ret);
}

void Texture2D::Use(const NNUInt& slot) {
	Utils::getContext()->PSSetShaderResources(slot, 1, &mpSRV);
	Utils::getContext()->PSSetSamplers(slot, 1, &mpSampler);
}

#endif