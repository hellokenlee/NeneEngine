/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_GL

#include "RenderContext.h"
#include "Utils.h"


DepthStencilState::DepthStencilState() {

}

void DepthStencilState::DepthTest(const bool& enabled) {
	if (enabled) {
		glEnable(GL_DEPTH_TEST);
	} else {
		glDisable(GL_DEPTH_TEST);
	}
}

void DepthStencilState::DepthMask(const bool& write) {
	
}

void DepthStencilState::DepthFunc(const NNTestFunc& func) {
	
}

void DepthStencilState::StencilTest(const bool& enabled) {
	
}

void DepthStencilState::StencilMask(const NNUInt& mask) {
	
}

void DepthStencilState::StencilFunc(const NNTestFunc& func, const NNUInt& ref, const NNUInt& mask) {

}

void DepthStencilState::StencilOp(const NNStencilOp& sfail, const NNStencilOp& dpfail, const NNStencilOp& dppass) {

}

void DepthStencilState::Commit() {
	
}


void RasterizerState::FillMode(const NNFillMode mode)
{

}

void RasterizerState::CullMode(const NNCullMode mode)
{
	if (mode == CULL_NONE)
	{
		glDisable(GL_CULL_FACE);
	}
	else
	{
		glEnable(GL_CULL_FACE);
		switch (mode)
		{
		case CULL_BACK:
			glCullFace(GL_BACK);
			break;
		case CULL_FRONT:
			glCullFace(GL_FRONT);
			break;
		case CULL_ALL:
			glCullFace(GL_FRONT_AND_BACK);
			break;
		}
	}
}
void RasterizerState::FrontFace(const NNVertexOrder order)
{

}

void RasterizerState::DepthBias(const NNFloat bias, const NNFloat slope)
{

}



RenderContext& RenderContext::Instance() {
	static RenderContext ins;
	return ins;
}

RenderContext::RenderContext() :
	m_blend(new BlendState()),
	m_rasterizer(new RasterizerState()),
	m_depth_stencil(new DepthStencilState())
{}

void RenderContext::Commit() {
	m_blend->Commit();
	m_rasterizer->Commit();
	m_depth_stencil->Commit();
}

#endif // NENE_GL