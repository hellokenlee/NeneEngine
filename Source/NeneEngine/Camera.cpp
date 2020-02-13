/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/

#include "Camera.h"

Camera::Camera()
{
	// 初始化视角矩阵
	m_up = NNVec3(0.0f, 1.0f, 0.0f);
	m_front = NNVec3(0.0f, 0.0f, -1.0f);
	m_position = NNVec3(0.0f, 0.0f, 3.0f);
	m_right = NNNormalize(NNCross(m_front, m_up));
	m_view_mat = NNCreateLookAt(m_position, m_position + m_front, m_up);
	// 初始化投影矩阵
	m_orthographic = false;
	m_pers_fov = NNRadians(45.0f);
	m_pers_ratio = (NNFloat)Utils::GetWindowWidth() / (NNFloat)Utils::GetWindowHeight();
	m_near = 0.0001f;
	m_far = 1000.0f;
	m_proj_mat = NNCreatePerspective(m_pers_fov, m_pers_ratio, m_near, m_far);
}

Camera::~Camera() {}

void Camera::Rotate(const NNFloat pitch, const NNFloat yaw) 
{
	// 更新前向量
	m_front.x = cosf(yaw) * cosf(pitch);
	m_front.y = sinf(pitch);
	m_front.z = sinf(yaw) * cosf(pitch);
	m_front = NNNormalize(m_front);
	// 更新视角矩阵
	m_view_mat = NNCreateLookAt(m_position, m_position + m_front, m_up);
	// 更新右向量
	m_right = NNCross(m_front, m_up);
	m_right = NNNormalize(m_right);
}

void Camera::SetPerspective(const NNFloat fov, const NNFloat ratio, const NNFloat nearz, const NNFloat farz)
{
	m_pers_fov = fov;
	m_pers_ratio = ratio;
	m_near = nearz;
	m_far = farz;
	m_proj_mat = NNCreatePerspective(m_pers_fov, m_pers_ratio, m_near, m_far);
	m_orthographic = false;
}

void Camera::SetOrtho(const NNFloat left, const NNFloat right, const NNFloat bottom, const NNFloat top, const NNFloat near, const NNFloat far)
{
	m_orth_top = top;
	m_orth_left = left;
	m_orth_right = right;
	m_orth_bottom = bottom;
	m_far = far;
	m_near = near;
	m_proj_mat = NNCreateOrtho(m_orth_left, m_orth_right, m_orth_bottom, m_orth_top, m_near, m_far);
	m_orthographic = true;
}

void Camera::Use() {
	// 更新View矩阵
	m_view_mat = NNCreateLookAt(m_position, m_position + m_front, m_up);
	// 写入常量缓冲
	NeneCB& CB = NeneCB::Instance();
	CB.PerFrame().data.view = m_view_mat;
	CB.PerFrame().data.projection = m_proj_mat;
	CB.PerFrame().data.camera_position = m_position;
}
