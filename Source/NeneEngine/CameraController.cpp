/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/

#include "CameraController.h"
#include "Types.h"
#include "Mouse.h"
#include "Keyboard.h"

using namespace std;

CameraController::CameraController() :
	m_yaw(NNRadians(270.0f)),
	m_pitch(NNRadians(0.0f)),
	m_speed(1.0f),
	m_sensitivity(0.001f),
	m_is_lock(false),
	m_is_first_move(true),
	m_camera(new Camera()),
	m_last_time(Utils::GetTime()),
	m_delta_x(0.0f),
	m_delta_y(0.0f),
	m_delta_time(0.0f) {
	Mouse::Instance().OnMove().AddObserver(this);
	Mouse::Instance().OnClick().AddObserver(this);
	Keyboard::Instance().OnPress().AddObserver(this);
}

CameraController::CameraController(shared_ptr<Camera> pCam) :
	m_yaw(NNRadians(270.0f)),
	m_pitch(NNRadians(-90.0f)),
	m_speed(0.5f),
	m_sensitivity(0.05f),
	m_is_lock(false),
	m_is_first_move(true),
	m_camera(pCam),
	m_last_time(Utils::GetTime()) {
	Mouse::Instance().OnMove().AddObserver(this);
	Keyboard::Instance().OnPress().AddObserver(this);
}

CameraController::~CameraController() {
	Mouse::Instance().OnMove().DelObserver(this);
	Keyboard::Instance().OnPress().AddObserver(this);
}

shared_ptr<CameraController> CameraController::Create() {
	CameraController* ret = new CameraController();
	return shared_ptr<CameraController>(ret);
}

shared_ptr<CameraController> CameraController::Create(std::shared_ptr<Camera> cam) {
	CameraController* ret = new CameraController(cam);
	return shared_ptr<CameraController>(ret);
}

void CameraController::Update() {
	//
	m_curr_time = Utils::GetTime();
	m_delta_time = m_curr_time - m_last_time;
	m_last_time = m_curr_time;
	//
	m_distance = m_delta_time * m_speed;
	static Keyboard& kb = Keyboard::Instance();
	//
	if (kb.GetKey(NNKeyMap(W)) == PRESSED)
		m_camera->MoveForward(m_distance);
	if (kb.GetKey(NNKeyMap(A)) == PRESSED)
		m_camera->MoveLeft(m_distance);
	if (kb.GetKey(NNKeyMap(S)) == PRESSED)
		m_camera->MoveBack(m_distance);
	if (kb.GetKey(NNKeyMap(D)) == PRESSED)
		m_camera->MoveRight(m_distance);
}

void CameraController::SetLocked(const bool& isLocked) {
	m_is_lock = isLocked;
}

void CameraController::OnNotify(shared_ptr<BaseEvent> eve) {
	//
	switch (eve->mCode)
	{
	case ON_MOUSE_MOVE:
	{
		//
		if (m_is_lock) 
		{
			break;
		}
		shared_ptr<MouseMoveEvent> m_event = dynamic_pointer_cast<MouseMoveEvent>(eve);
		//
		m_curr_x = m_event->mdx;
		m_curr_y = m_event->mdy;
		//
		if (m_is_first_move) 
		{
			m_last_x = m_curr_x;
			m_last_y = m_curr_y;
			m_is_first_move = false;
		}
		//
		m_delta_x = (m_curr_x - m_last_x) * m_sensitivity;
		m_delta_y = (m_curr_y - m_last_y) * m_sensitivity;
		m_last_x = m_curr_x;
		m_last_y = m_curr_y;
		//
		m_yaw += m_delta_x;
		m_pitch -= m_delta_y;
		if (m_pitch > NNRadians(89.5f)) {
			m_pitch = NNRadians(89.5f);
		}
		else if (m_pitch < NNRadians(-89.5)) {
			m_pitch = NNRadians(-89.5f);
		}
		if (m_yaw > NNRadians(360.0f)) {
			m_yaw -= NNRadians(360.0f);
		}
		else if (m_yaw < NNRadians(0.0f)) {
			m_yaw += NNRadians(360.0f);
		}
		m_camera->Rotate(m_pitch, m_yaw);
		break;
	}
	case ON_KEY_PRESS:
	{
		shared_ptr<KeyboardEvent> k_event = dynamic_pointer_cast<KeyboardEvent>(eve);
		if (k_event->mKey == NNKeyMap(C)) {
			m_is_lock = !m_is_lock;
			m_is_first_move = true;
			Mouse::Instance().SetCursorShow(m_is_lock);
		}
		break;
	}
	default:
		break;
	}
}


