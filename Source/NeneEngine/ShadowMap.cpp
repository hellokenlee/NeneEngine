#include "ShadowMap.h"
#include "NeneCB.h"

using namespace std;

shared_ptr<ShadowMap> ShadowMap::Create(const NNFloat& radius, const NNUInt& width, const NNUInt& height)
{
	ShadowMap* ret = new ShadowMap(radius, width, height);
	if (!ret->m_valid)
	{
		delete ret;
		return nullptr;
	}
	return shared_ptr<ShadowMap>(ret);
}

ShadowMap::ShadowMap(const NNFloat& radius, const NNUInt& width, const NNUInt& height) :
	RenderTarget(width, height, 1), m_radius(radius), m_distance(1.0f)
{}

void ShadowMap::SetLight(std::shared_ptr<Light> shadowlight)
{
	m_light = shadowlight;
}

void ShadowMap::SetRadius(const NNFloat rad)
{
	m_radius = rad;
}

void ShadowMap::SetDistance(const NNFloat dis)
{
	m_distance = dis;
}

void ShadowMap::Begin()
{
	// 设置视点
	Utils::SetViewPort(0, 0, m_width, m_height);
	// 计算光源位置
	NNVec3 cam_pos = NeneCB::Instance().PerFrame().data.camera_position;
	NNVec3 light_target = (NNNormalize(m_light->GetDirection()) * m_distance) + cam_pos;
	// 计算光源空间矩阵
	m_light_view_mat = NNCreateLookAt(cam_pos, light_target, NNVec3(0.0f, 1.0f, 0.0f));
	m_light_proj_mat = NNCreateOrtho(-m_radius, m_radius, -m_radius, m_radius, 0.1f, 1000.5f);
	//
	NeneCB::Instance().PerFrame().data.shadowlight_view = m_light_view_mat;
	NeneCB::Instance().PerFrame().data.shadowlight_proj = m_light_proj_mat;
	NeneCB::Instance().PerFrame().data.shadowlight_space = m_light_proj_mat * m_light_view_mat;
	NeneCB::Instance().PerFrame().Update(PER_FRAME_SLOT);
	//
	RenderTarget::Begin();
}

void ShadowMap::End()
{
	//
	RenderTarget::End();
	//
	Utils::SetViewPort(0, 0, Utils::GetWindowWidth(), Utils::GetWindowHeight());
}

void ShadowMap::Use(const NNUInt& slot)
{
	RenderTarget::GetDepthStencilTex()->Use(slot);
}
