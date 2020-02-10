/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_DX

#include "IO.h"
#include "Debug.h"
#include "TextureCube.h"

enum CubeMapBias {
	BIAS_RIGHT = 0,
	BIAS_LEFT,
	BIAS_TOP,
	BIAS_BOTTOM,
	BIAS_BACK,
	BIAS_FRONT,
	CubeMapBiasNum
};

using namespace std;

TextureCube::TextureCube() : Texture(), mpSRV(nullptr), mpSampler(nullptr) {}

TextureCube::~TextureCube() {
	if (mpTex) mpTex->Release();
	if (mpSRV) mpSRV->Release();
	if (mpSampler) mpSampler->Release();
}

shared_ptr<TextureCube> TextureCube::Create(
	const NNChar* filepathTop, const NNChar* filepathBottom,
	const NNChar* filepathLeft, const NNChar* filepathRight,
	const NNChar* filepathFront, const NNChar* filepathBack
) {
	//
	shared_ptr<NNByte[]> imageDatas[CubeMapBiasNum];
	UINT width[CubeMapBiasNum];
	UINT height[CubeMapBiasNum];
	NNColorFormat format[CubeMapBiasNum];
	//
	imageDatas[BIAS_RIGHT] = loadImage(filepathRight, width[BIAS_RIGHT], height[BIAS_RIGHT], format[BIAS_RIGHT]);
	imageDatas[BIAS_LEFT] = loadImage(filepathLeft, width[BIAS_LEFT], height[BIAS_LEFT], format[BIAS_LEFT]);
	imageDatas[BIAS_FRONT] = loadImage(filepathFront, width[BIAS_FRONT], height[BIAS_FRONT], format[BIAS_FRONT]);
	imageDatas[BIAS_BACK] = loadImage(filepathBack, width[BIAS_BACK], height[BIAS_BACK], format[BIAS_BACK]);
	imageDatas[BIAS_TOP] = loadImage(filepathTop, width[BIAS_TOP], height[BIAS_TOP], format[BIAS_TOP]);
	imageDatas[BIAS_BOTTOM] = loadImage(filepathBottom, width[BIAS_BOTTOM], height[BIAS_BOTTOM], format[BIAS_BOTTOM]);
	//
	for (int i = 1; i < CubeMapBiasNum; ++i) {
		if (width[i] != width[i - 1] || height[i] != height[i - 1] || format[i] != format[i - 1]) {
			dLog("TextureCube requires all textures has same height, width and format!");
			return nullptr;
		}
	}
	//
	ID3D11Texture2D* pTexCube;
	D3D11_TEXTURE2D_DESC texCubeDesc;
	ZeroMemory(&texCubeDesc, sizeof(texCubeDesc));
	texCubeDesc.Width = width[0];
	texCubeDesc.Height = height[0];
	texCubeDesc.MipLevels = 1;
	texCubeDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texCubeDesc.ArraySize = CubeMapBiasNum;
	texCubeDesc.SampleDesc.Count = 1;
	texCubeDesc.SampleDesc.Quality = 0;
	texCubeDesc.Usage = D3D11_USAGE_DEFAULT;
	texCubeDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texCubeDesc.CPUAccessFlags = 0;
	texCubeDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	//
	D3D11_SUBRESOURCE_DATA initDatas[CubeMapBiasNum];
	for (int i = 0; i < CubeMapBiasNum; ++i) {
		initDatas[i].pSysMem = imageDatas[i].get();
		initDatas[i].SysMemPitch = static_cast<UINT>(width[i] * 4);
		initDatas[i].SysMemSlicePitch = static_cast<UINT>(width[i] * height[i] * 4);
	}
	//
	HRESULT hr = Utils::getDevice()->CreateTexture2D(&texCubeDesc, &initDatas[0], &pTexCube);
	if (FAILED(hr)) {
		dLog("[Error] Failed to Create texture 2D!\n");
		return nullptr;
	}
	//
	ID3D11ShaderResourceView* pSRV;
	D3D11_SHADER_RESOURCE_VIEW_DESC texCubeViewDesc;
	texCubeViewDesc.Format = texCubeDesc.Format;
	texCubeViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	texCubeViewDesc.TextureCube.MostDetailedMip = 0;
	texCubeViewDesc.TextureCube.MipLevels = texCubeDesc.MipLevels;
	//
	hr = Utils::getDevice()->CreateShaderResourceView(pTexCube, &texCubeViewDesc, &pSRV);
	if (FAILED(hr)) {
		pTexCube->Release();
		dLog("[Error] Failed to Create SRV!\n");
		return nullptr;
	}
	//
	ID3D11SamplerState* pSampler;
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	// 创建采样器
	hr = Utils::getDevice()->CreateSamplerState(&samplerDesc, &pSampler);
	if (FAILED(hr)) {
		pSRV->Release();
		pTexCube->Release();
		return nullptr;
	}
	//
	TextureCube* ret = new TextureCube();
	ret->mpTex = pTexCube;
	ret->mpSRV = pSRV;
	ret->mpSampler = pSampler;
	//
	return shared_ptr<TextureCube>(ret);
}

void TextureCube::Use(const NNUInt& slot) {
	Utils::getContext()->PSSetShaderResources(slot, 1, &mpSRV);
	Utils::getContext()->PSSetSamplers(slot, 1, &mpSampler);
}

#endif // NENE_GL