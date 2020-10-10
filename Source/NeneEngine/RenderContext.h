/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef RENDER_CONTEXT_H
#define RENDER_CONTEXT_H

#include <memory>
#include "Types.h"


class RenderContextState {
public:
	RenderContextState() : mDirty(true) {}
	virtual void commit() = 0;
	virtual bool dirty() { return mDirty; }
protected:
	bool mDirty;
private:
	friend class RenderContext;
};

class DepthStencilState : public RenderContextState {
public:
	//
	DepthStencilState();
	//
	void depthTest(const bool& enabled);
	void depthMask(const bool& write);
	void depthFunc(const NNTestFunc& func);
	//
	void stencilTest(const bool& enabled);
	void stencilMask(const NNUInt& mask);
	void stencilFunc(const NNTestFunc& func, const NNUInt& ref, const NNUInt& mask);
	void stencilOp(const NNStencilOp& sfail, const NNStencilOp& dpfail, const NNStencilOp& dppass);
	//
	virtual void commit();
protected:
#if defined NENE_GL
	;
#elif defined NENE_DX
	UINT mStencilRef;
	ID3D11DepthStencilState *mpDSState;
	D3D11_DEPTH_STENCIL_DESC mDSSDesc;
#endif
};

class BlendState : public RenderContextState {
public:
	virtual void commit() {}
};

class RasterizerState : public RenderContextState {
public:
	virtual void commit() {}
};

class RenderContext {
public:
	//
	static RenderContext& instance();
	//
	void commit();
	//
	inline std::shared_ptr<DepthStencilState> pDepthStencil() { return mpDepthStencil; }
	inline void setDepthStencil(std::shared_ptr<DepthStencilState> new_state) { new_state->mDirty = true; mpDepthStencil = new_state; }
	//
	inline std::shared_ptr<BlendState> pBlend() { return mpBlend; }
	inline void setBlend(std::shared_ptr<BlendState> new_state) { new_state->mDirty = true; mpBlend = new_state; }
	//
	inline std::shared_ptr<RasterizerState> pRasterizer() { return mpRasterizer; }
	inline void setRasterizer(std::shared_ptr<RasterizerState> new_state) { new_state->mDirty = true; mpRasterizer = new_state; }
protected:
	std::shared_ptr<BlendState> mpBlend;
	std::shared_ptr<RasterizerState> mpRasterizer;
	std::shared_ptr<DepthStencilState> mpDepthStencil;
private:
	//
	RenderContext();
	RenderContext(RenderContext const&) = delete;
	void operator=(RenderContext const&) = delete;
};

#endif // RENDER_CONTEXT_H