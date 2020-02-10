/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_DX

#include "RenderTarget.h"
#include "Debug.h"

using namespace std;

RenderTarget::RenderTarget() {}

RenderTarget::~RenderTarget() {
	
}

shared_ptr<RenderTarget> RenderTarget::Create(const NNUInt& width, const NNUInt& height, const NNUInt& count) {
	HRESULT res = S_OK;
	//
	RenderTarget* ret = new RenderTarget();
	ret->mCount = count;
	ret->mWidth = width;
	ret->mHeight = height;
	ret->mColorTexes.resize(count);
	// 创建颜色附件
	for (NNUInt i = 0; i < count; ++i) {
		// 创建 TEX
		ID3D11Texture2D *pTEX;
		D3D11_TEXTURE2D_DESC TEXDesc;
		TEXDesc.Width = width;
		TEXDesc.Height = height;
		TEXDesc.MipLevels = 1;
		TEXDesc.ArraySize = 1;
		TEXDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		TEXDesc.SampleDesc.Count = 1;
		TEXDesc.SampleDesc.Quality = 0;
		TEXDesc.Usage = D3D11_USAGE_DEFAULT;
		TEXDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		TEXDesc.CPUAccessFlags = 0;
		TEXDesc.MiscFlags = 0;
		res = Utils::getDevice()->CreateTexture2D(&TEXDesc, nullptr, &pTEX);
		if (FAILED(res)) {
			dLog("Failed to Create Texture 2D!\n");
			delete ret;
			return nullptr;
		}
		// 创建 SRV
		ID3D11ShaderResourceView *pSRV;
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.Format = TEXDesc.Format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		SRVDesc.Texture2D.MipLevels = 1;
		res = Utils::getDevice()->CreateShaderResourceView(pTEX, &SRVDesc, &pSRV);
		if (FAILED(res)) {
			dLog("Failed to Create Shader Resource View!\n");
			delete ret;
			return nullptr;
		}
		// 创建 SAMPLER
		ID3D11SamplerState* pSampler;
		D3D11_SAMPLER_DESC SLPDesc;
		ZeroMemory(&SLPDesc, sizeof(SLPDesc));
		SLPDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		SLPDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		SLPDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		SLPDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		SLPDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		SLPDesc.MinLOD = 0;
		SLPDesc.MaxLOD = D3D11_FLOAT32_MAX;
		res = Utils::getDevice()->CreateSamplerState(&SLPDesc, &pSampler);
		if (FAILED(res)) {
			dLog("Failed to Create Texture Sampler!");
			delete ret;
			return nullptr;
		}
		// 创建 RTV
		ID3D11RenderTargetView* pRTV;
		D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
		ZeroMemory(&RTVDesc, sizeof(RTVDesc));
		RTVDesc.Format = TEXDesc.Format;
		RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		RTVDesc.Texture2D.MipSlice = 0;
		HRESULT res = Utils::getDevice()->CreateRenderTargetView(pTEX, &RTVDesc, &pRTV);
		if (FAILED(res)) {
			dLog("Failed to Create Render Target View!");
			delete ret;
			return nullptr;
		}
		// 赋值
		ret->mColorTexes[i]->mpSampler = pSampler;
		ret->mColorTexes[i]->mpSRV = pSRV;
		ret->mRTVs[i] = pRTV;
	}
	// 创建深度模板附件
	// 创建 TEX
	ID3D11Texture2D* pTEX;
	D3D11_TEXTURE2D_DESC TEXDesc;
	TEXDesc.Width = width;
	TEXDesc.Height = height;
	TEXDesc.MipLevels = 1;
	TEXDesc.ArraySize = 1;
	TEXDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	TEXDesc.SampleDesc.Count = 1;
	TEXDesc.SampleDesc.Quality = 0;
	TEXDesc.Usage = D3D11_USAGE_DEFAULT;
	TEXDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	TEXDesc.CPUAccessFlags = 0;
	TEXDesc.MiscFlags = 0;
	res = Utils::getDevice()->CreateTexture2D(&TEXDesc, nullptr, &pTEX);
	if (FAILED(res)) {
		dLog("Failed to Create Texture 2D!");
		delete ret;
		return nullptr;
	}
	// 创建 SRV
	ID3D11ShaderResourceView *pSRV;
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	SRVDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = TEXDesc.MipLevels;
	SRVDesc.Texture2D.MostDetailedMip = 0;
	res = Utils::getDevice()->CreateShaderResourceView(pTEX, &SRVDesc, &pSRV);
	if (FAILED(res)) {
		dLog("Failed to Create Shader Resource View!");
		delete ret;
		return nullptr;
	}
	// 创建 SAMPLER
	ID3D11SamplerState* pSampler;
	D3D11_SAMPLER_DESC SLPDesc;
	ZeroMemory(&SLPDesc, sizeof(SLPDesc));
	SLPDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SLPDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SLPDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	SLPDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	SLPDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	SLPDesc.BorderColor[0] = 1.0f;
	SLPDesc.BorderColor[1] = 1.0f;
	SLPDesc.BorderColor[2] = 1.0f;
	SLPDesc.BorderColor[3] = 1.0f;
	SLPDesc.MinLOD = 0;
	SLPDesc.MaxLOD = D3D11_FLOAT32_MAX;
	res = Utils::getDevice()->CreateSamplerState(&SLPDesc, &pSampler);
	if (FAILED(res)) {
		dLog("Failed to Create Sapmler!");
		delete ret;
		return nullptr;
	}
	// 创建DSV
	ID3D11DepthStencilView* pDSV;
	D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc;
	DSVDesc.Flags = 0;
	DSVDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DSVDesc.Texture2D.MipSlice = 0;
	HRESULT hr2 = Utils::getDevice()->CreateDepthStencilView(pTEX, &DSVDesc, &pDSV);
	if (FAILED(res)) {
		dLog("Failed to Create Depth Stencil View!");
		delete ret;
		return nullptr;
	}
	//
	ret->mDepthStencilTex->mpSampler = pSampler;
	ret->mDepthStencilTex->mpSRV = pSRV;
	ret->mpDSV = pDSV;
	//
	return shared_ptr<RenderTarget>(ret);
}

const shared_ptr<Texture2D> RenderTarget::getColorTex(const NNUInt& idx) {
	if (idx < mColorTexes.size()) {
		return mColorTexes[idx];
	}
	return nullptr;
}

const shared_ptr<Texture2D> RenderTarget::getDepthStencilTex() {
	return mDepthStencilTex;
}

void RenderTarget::begin() {
	//
	mPreRTVCount = Utils::mRTVCount;
	// 保存渲染对象
	Utils::getContext()->OMGetRenderTargets(mPreRTVCount, &mpPrevRTV, &mpPrevDSV);
	// 设置渲染对象
	Utils::getContext()->OMSetRenderTargets(mCount, mRTVs.data(), mpDSV);
}

void RenderTarget::end() {
	//
	Utils::mRTVCount = mPreRTVCount;
	// 恢复原来的渲染对象
	Utils::getContext()->OMSetRenderTargets(mPreRTVCount, &mpPrevRTV, mpPrevDSV);
}

#endif // NENE_DX