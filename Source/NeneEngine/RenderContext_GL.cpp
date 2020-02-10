/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_GL

#include "RenderContext.h"
#include "Utils.h"


DepthStencilState::DepthStencilState() {

}

void DepthStencilState::depthTest(const bool& enabled) {
	if (enabled) {
		glEnable(GL_DEPTH_TEST);
	} else {
		glDisable(GL_DEPTH_TEST);
	}
}

void DepthStencilState::depthMask(const bool& write) {
	
}

void DepthStencilState::depthFunc(const NNTestFunc& func) {
	
}

void DepthStencilState::stencilTest(const bool& enabled) {
	
}

void DepthStencilState::stencilMask(const NNUInt& mask) {
	
}

void DepthStencilState::stencilFunc(const NNTestFunc& func, const NNUInt& ref, const NNUInt& mask) {

}

void DepthStencilState::stencilOp(const NNStencilOp& sfail, const NNStencilOp& dpfail, const NNStencilOp& dppass) {

}

void DepthStencilState::commit() {
	
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

#endif // NENE_GL