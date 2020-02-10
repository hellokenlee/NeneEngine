/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_DX

#include "RenderContext.h"
#include "Utils.h"

DepthStencilState::DepthStencilState() : mStencilRef(0), mpDSState(nullptr) {
	ZeroMemory(&mDSSDesc, sizeof(mDSSDesc));
}

void DepthStencilState::depthTest(const bool& enabled) {
	if (enabled != (bool)(mDSSDesc.DepthEnable)) {
		mDSSDesc.DepthEnable = enabled;
		mDirty = true;
	}
}

void DepthStencilState::depthMask(const bool& write) {
	D3D11_DEPTH_WRITE_MASK mask = write ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	if (mask != mDSSDesc.DepthWriteMask) {
		mDSSDesc.DepthWriteMask = mask;
		mDirty = true;
	}
}

void DepthStencilState::depthFunc(const NNTestFunc& func) {
	if (func != mDSSDesc.DepthFunc) {
		mDSSDesc.DepthFunc = (D3D11_COMPARISON_FUNC)func;
		mDirty = true;
	}
}

void DepthStencilState::stencilTest(const bool& enabled) {
	if (enabled != (bool)(mDSSDesc.StencilEnable)) {
		mDSSDesc.StencilEnable = enabled;
		mDirty = true;
	}
}

void DepthStencilState::stencilMask(const NNUInt& mask) {
	if (mask != mDSSDesc.StencilWriteMask) {
		mDSSDesc.StencilWriteMask = mask;
		mDirty = true;
	}
}

void DepthStencilState::stencilFunc(const NNTestFunc& func, const NNUInt& ref, const NNUInt& mask) {

}

void DepthStencilState::stencilOp(const NNStencilOp& sfail, const NNStencilOp& dpfail, const NNStencilOp& dppass) {

}

void DepthStencilState::commit() {
	if (mDirty) {
		ID3D11DepthStencilState *pNewDssState;
		//
		Utils::getDevice()->CreateDepthStencilState(&mDSSDesc, &pNewDssState);
		//
		Utils::getContext()->OMSetDepthStencilState(pNewDssState, mStencilRef);
		//
		if (mpDSState) {
			mpDSState->Release();
		}
		//
		mpDSState = pNewDssState;
		//
		mDirty = false;
	}
}

RenderContext& RenderContext::instance() {
	static RenderContext ins;
	return ins;
}

RenderContext::RenderContext() : 
	mpBlend(new BlendState()), 
	mpRasterizer(new RasterizerState()), 
	mpDepthStencil(new DepthStencilState()) 
{}

void RenderContext::commit() {
	//
	mpBlend->commit();
	mpRasterizer->commit();
	mpDepthStencil->commit();
}

#endif // NENE_DX