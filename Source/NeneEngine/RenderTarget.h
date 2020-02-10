/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef RENDER_TARGET_H
#define RENDER_TARGET_H

#include "Texture2D.h"
#include "Types.h"
#include <vector>
#include <memory>

//
//    RenderTarget: 
//

class RenderTarget : public std::enable_shared_from_this<RenderTarget> {
public:
	//
	static std::shared_ptr<RenderTarget> Create(const NNUInt& width, const NNUInt& height, const NNUInt& count);
	//
	static void Blit(const RenderTarget& src, const RenderTarget& dest, NNUInt field, NNUInt filter);
	//
	virtual ~RenderTarget();
	//
	const std::shared_ptr<Texture2D> GetColorTex(const NNUInt& idx = 0);
	const std::shared_ptr<Texture2D> GetDepthStencilTex();
	//
	virtual void Begin();
	virtual void End();
protected:
	//
	bool m_valid;
	//
	NNUInt m_count;;
	NNUInt m_width, m_height;
	//
	std::shared_ptr<Texture2D> m_depthstencil_tex;
	std::vector<std::shared_ptr<Texture2D>> m_color_texes;
#if defined NENE_GL
	GLuint m_fbo;
#elif defined NENE_DX
	//
	std::vector<ID3D11RenderTargetView*> mRTVs;
	ID3D11DepthStencilView* mpDSV;
	//
	NNUInt mPreRTVCount;
	ID3D11RenderTargetView* mpPrevRTV;
	ID3D11DepthStencilView* mpPrevDSV;
#endif
protected:
	RenderTarget() = delete;
	RenderTarget(const RenderTarget& rhs) = delete;
	RenderTarget& operator=(const RenderTarget& rhs) = delete;
	RenderTarget(const NNUInt& width, const NNUInt& height, const NNUInt& count);
};

#endif // RENDER_TARGET_H