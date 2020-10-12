/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/

#include "Camera.h"

Camera::Camera() {
	// 初始化视角矩阵
	m_up = NNVec3(0.0f, 1.0f, 0.0f);
	m_front = NNVec3(0.0f, 0.0f, -1.0f);
	m_position = NNVec3(0.0f, 0.0f, 3.0f);
	m_right = NNNormalize(NNCross(m_front, m_up));
	m_view_mat = NNCreateLookAt(m_position, m_position + m_front, m_up);
	// 初始化投影矩阵
	m_fov = NNRadians(45.0f);
	m_ratio = (NNFloat)Utils::GetWindowWidth() / (NNFloat)Utils::GetWindowHeight();
	m_near = 0.0001f;
	m_far = 1000.0f;
	m_proj_mat = NNCreatePerspective(m_fov, m_ratio, m_near, m_far);
}

Camera::~Camera() {

}

void Camera::MoveUp(const NNFloat& distance) {
	m_position += distance * m_up;
}

void Camera::MoveDowm(const NNFloat& distance) {
	m_position -= distance * m_up;
}

void Camera::MoveLeft(const NNFloat& distance) {
	m_position -= distance * m_right;
}

void Camera::MoveRight(const NNFloat& distance) {
	m_position += distance * m_right;
}

void Camera::MoveBack(const NNFloat& distance) {
	m_position -= distance * m_front;
}

void Camera::MoveForward(const NNFloat& distance) {
	m_position += distance * m_front;
}

void Camera::Rotate(const NNFloat& pitch, const NNFloat& yaw) {
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

void Camera::SetPerspective(const NNFloat& fov, const NNFloat& ratio, const NNFloat& nearz, const NNFloat& farz) {
	m_fov = fov;
	m_ratio = ratio;
	m_near = nearz;
	m_far = farz;
	m_proj_mat = NNCreatePerspective(m_fov, m_ratio, m_near, m_far);
}

void Camera::SetOrtho() {

}

void Camera::Use() {
	// 更新View矩阵
	m_view_mat = NNCreateLookAt(m_position, m_position + m_front, m_up);
	// 写入常量缓冲
	NeneCB& CB = NeneCB::Instance();
	CB.PerFrame().Data().view = m_view_mat;
	CB.PerFrame().Data().projection = m_proj_mat;
	CB.PerFrame().Data().camera_position = m_position;
}
