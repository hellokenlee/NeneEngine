/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef RENDER_CONTEXT_H
#define RENDER_CONTEXT_H

#include <memory>
#include "Types.h"


class RenderContextState 
{
public:
	RenderContextState() : m_dirty(true) {}
	virtual void Commit() = 0;
	virtual inline bool Dirty() { return m_dirty; }
protected:
	bool m_dirty;

	friend class RenderContext;
};

class DepthStencilState : public RenderContextState 
{
public:
	//
	DepthStencilState();
	//
	void DepthTest(const bool& enabled);
	void DepthMask(const bool& write);
	void DepthFunc(const NNTestFunc& func);
	//
	void StencilTest(const bool& enabled);
	void StencilMask(const NNUInt& mask);
	void StencilFunc(const NNTestFunc& func, const NNUInt& ref, const NNUInt& mask);
	void StencilOp(const NNStencilOp& sfail, const NNStencilOp& dpfail, const NNStencilOp& dppass);
	//
	virtual void Commit() override;
protected:
#if defined NENE_GL
	;
#elif defined NENE_DX
	UINT mStencilRef;
	ID3D11DepthStencilState *mpDSState;
	D3D11_DEPTH_STENCIL_DESC mDSSDesc;
#endif
};

class BlendState : public RenderContextState 
{
public:
	virtual void Commit() override {}
};

class RasterizerState : public RenderContextState 
{
public:
	//
	void FillMode(const NNFillMode mode);
	//
	void CullMode(const NNCullMode mode);
	void FrontFace(const NNVertexOrder order);
	//
	void DepthBias(const NNFloat bias, const NNFloat slope);
	//
	virtual void Commit() override {}
};

class RenderContext {
public:
	//
	static RenderContext& Instance();
	//
	void Commit();
	//
	inline std::shared_ptr<DepthStencilState> DepthStencil() { return m_depth_stencil; }
	inline void SetDepthStencil(std::shared_ptr<DepthStencilState> new_state) { new_state->m_dirty = true; m_depth_stencil = new_state; }
	//
	inline std::shared_ptr<BlendState> Blend() { return m_blend; }
	inline void setBlend(std::shared_ptr<BlendState> new_state) { new_state->m_dirty = true; m_blend = new_state; }
	//
	inline std::shared_ptr<RasterizerState> Rasterizer() { return m_rasterizer; }
	inline void setRasterizer(std::shared_ptr<RasterizerState> new_state) { new_state->m_dirty = true; m_rasterizer = new_state; }
protected:
	std::shared_ptr<BlendState> m_blend;
	std::shared_ptr<RasterizerState> m_rasterizer;
	std::shared_ptr<DepthStencilState> m_depth_stencil;
private:
	//
	RenderContext();
	RenderContext(RenderContext const&) = delete;
	void operator=(RenderContext const&) = delete;
};

#endif // RENDER_CONTEXT_H