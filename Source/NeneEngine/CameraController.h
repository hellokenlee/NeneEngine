/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include <memory>
#include "Camera.h"
#include "Observer.h"

//
//    CameraController: Control Your Camera
//

class CameraController : public Observer 
{
public:
	//
	virtual ~CameraController();
	//
	static std::shared_ptr<CameraController> Create();
	static std::shared_ptr<CameraController> Create(std::shared_ptr<Camera> cam);
	// 
	void Update();
	inline void SetSpeed(const NNFloat speed) { m_speed = speed; }
	inline void SetSensitivity(const NNFloat sense) { m_sensitivity = sense; };
	//
	void SetLocked(const bool& isLocked);
	// 获取摄像机
	inline const std::shared_ptr<Camera> GetCamera() { return m_camera; }

	// 移动摄像机
	void MoveUp(const NNFloat distance);
	void MoveDowm(const NNFloat distance);
	void MoveLeft(const NNFloat distance);
	void MoveRight(const NNFloat distance);
	void MoveBack(const NNFloat distance);
	void MoveForward(const NNFloat distance);
protected:
	//
	std::shared_ptr<Camera> m_camera;
	// 移动速度
	NNFloat m_speed;
	// 镜头灵敏度
	NNFloat m_sensitivity;
	//
	NNFloat m_yaw, m_pitch;
	bool m_is_lock, m_is_first_move;
	//
	NNFloat m_last_time, m_curr_time, m_delta_time;
	NNFloat m_distance;
	//
	NNFloat m_curr_x, m_curr_y, m_last_x, m_last_y, m_delta_x, m_delta_y;
protected:
	// 构造函数
	CameraController();
	CameraController(std::shared_ptr<Camera> cam);
	CameraController(const CameraController& rhs) = delete;
	CameraController& operator=(const CameraController& rhs) = delete;
	// 输入事件监听
	virtual void OnNotify(std::shared_ptr<BaseEvent> eve);
};


#endif // CAMERA_CONTROLLER_H