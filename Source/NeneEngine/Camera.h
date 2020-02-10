/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef CAMERA_H
#define CAMERA_H

#include <memory>

#include "Utils.h"
#include "NeneCB.h"

//
//    Camera: Mange View Point Class
//

class Camera : public std::enable_shared_from_this<Camera> {
public:
	// 构造
	Camera();
	// 析构
	~Camera();
	// 移动摄像机
	void MoveUp(const NNFloat& distance);
	void MoveDowm(const NNFloat& distance);
	void MoveLeft(const NNFloat& distance);
	void MoveRight(const NNFloat& distance);
	void MoveBack(const NNFloat& distance);
	void MoveForward(const NNFloat& distance);
	// 旋转摄像机
	void Rotate(const NNFloat& pitch, const NNFloat& yaw);
	// 设置为透视投影
	void SetPerspective(const NNFloat& fov, const NNFloat& ratio, const NNFloat& nearz, const NNFloat& farz);
	// 设置为正交投影
	void SetOrtho();
	// 使用该摄像机
	void Use();
	// 获取当前摄像机的矩阵
	inline NNMat4& GetViewMat() { return m_view_mat; };
	inline NNMat4& GetProjMat() { return m_proj_mat; };
	inline NNMat4& GetInversedViewMat() { m_inv_view_mat = NNMat4Inverse(m_view_mat); return m_inv_view_mat; };
	inline NNMat4& GetInversedProjMat() { m_inv_proj_mat = NNMat4Inverse(m_proj_mat); return m_inv_proj_mat; };
private:
	//
	NNMat4 m_view_mat;
	NNMat4 m_proj_mat;
	//
	NNVec3 m_up;
	NNVec3 m_front;
	NNVec3 m_right;
	NNVec3 m_position;
	//
	NNFloat m_fov, m_ratio, m_near, m_far;
	//
	NNMat4 m_inv_view_mat;
	NNMat4 m_inv_proj_mat;
};

#endif // CAMERA_H