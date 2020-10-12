#include "ShadowMap.h"
#include "NeneCB.h"

using namespace std;

shared_ptr<ShadowMap> ShadowMap::Create(const NNFloat& volume, const NNUInt& width, const NNUInt& height)
{
	//
	ShadowMap* ret = new ShadowMap();
	//
	//ret->RenderTarget::Create(width, height, 1);
	//
	ret->m_volume = volume;
	//
	return shared_ptr<ShadowMap>(ret);
}

void ShadowMap::Begin()
{
	NeneCB::Instance().PerFrame().Data().view = m_light_view_mat;
	NeneCB::Instance().PerFrame().Data().projection = m_light_proj_mat;
	NeneCB::Instance().PerFrame().Update(PER_FRAME_SLOT);

	RenderTarget::Begin();
}

void ShadowMap::End()
{
	RenderTarget::End();
}

void ShadowMap::Use(const NNUInt& slot)
{
	NeneCB::Instance().PerFrame().Data().shadowlight_space = m_light_view_mat * m_light_proj_mat;
	NeneCB::Instance().PerFrame().Update(PER_FRAME_SLOT);

	RenderTarget::GetDepthStencilTex()->Use(slot);
}
