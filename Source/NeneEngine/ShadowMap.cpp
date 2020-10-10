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

void ShadowMap::SetLight(std::shared_ptr<Light> shadowlight)
{
	m_light_view_mat = NNCreateLookAt(shadowlight->mDirection, NNVec3(0.0f, 0.0f, 0.0f), NNVec3(0.0f, -1.0f, 0.0f));
	m_light_proj_mat = NNCreateOrtho(-m_volume, m_volume, -m_volume, m_volume, 1.0f, 100.5f);
}

void ShadowMap::Begin()
{
	NeneCB::Instance().PerFrame().data.view = m_light_view_mat;
	NeneCB::Instance().PerFrame().data.projection = m_light_proj_mat;
	NeneCB::Instance().PerFrame().Update(PER_FRAME_SLOT);

	RenderTarget::Begin();
}

void ShadowMap::End()
{
	RenderTarget::End();
}

void ShadowMap::Use(const NNUInt& slot)
{
	auto& CB = NeneCB::Instance().PerFrame();
	CB.data.shadowlight_space = m_light_view_mat * m_light_proj_mat;
	CB.Update(PER_FRAME_SLOT);

	RenderTarget::GetDepthStencilTex()->Use(slot);
}
