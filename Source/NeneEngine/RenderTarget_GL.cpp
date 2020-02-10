/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_GL

#include "RenderTarget.h"
#include "Debug.h"

using namespace std;

RenderTarget::RenderTarget(const NNUInt& width, const NNUInt& height, const NNUInt& count) 
{
	//
	m_valid = true;
	//
	m_width = width;
	m_height = height;
	// 帧缓冲
	glGenFramebuffers(1, &m_fbo);
	// 颜色附件
	for (NNUInt i = 0; i < count; ++i) {
		m_color_texes.push_back(Texture2D::createFromMemory(width, height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE));
	}
	// 深度模板附件
	m_depthstencil_tex = Texture2D::createFromMemory(width, height, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
	// 绑定附件到帧缓冲
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		vector<unsigned int> attachments;
		for (unsigned int i = 0; i < count; ++i)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_color_texes[i]->mTextureID, 0);
			attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
		}
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthstencil_tex->mTextureID, 0);
		// 检查
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			m_valid = false;
			dLog("[Error] Framebuffer init failed!");
		}
	// 指定是否有多个颜色缓冲区 (Multiple Render Target)
	glDrawBuffers((NNUInt)attachments.size(), attachments.data());
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderTarget::~RenderTarget() {
	if (m_fbo != 0)
	{
		glDeleteBuffers(1, &m_fbo);
	}
}

shared_ptr<RenderTarget> RenderTarget::Create(const NNUInt& width, const NNUInt& height, const NNUInt& count) {
	//
	RenderTarget *ret = new RenderTarget(width, height, count);
	//
	if(!ret->m_valid)
	{
		delete ret;
		return nullptr;
	}
	//
	return shared_ptr<RenderTarget>(ret);
}

void RenderTarget::Blit(const RenderTarget& src, const RenderTarget& dest, NNUInt field, NNUInt filter) {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dest.m_fbo);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, src.m_fbo);
	glDrawBuffer(GL_BACK);
	glBlitFramebuffer(0, 0,src.m_width, src.m_height, 0, 0, dest.m_width, dest.m_height, field, filter);
}


const shared_ptr<Texture2D> RenderTarget::GetColorTex(const NNUInt& idx) {
	if (idx < m_color_texes.size()) {
		return m_color_texes[idx];
	}
	return nullptr;
}

const shared_ptr<Texture2D> RenderTarget::GetDepthStencilTex() {
	return m_depthstencil_tex;
}

void RenderTarget::Begin() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void RenderTarget::End() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



#endif // NENE_GL