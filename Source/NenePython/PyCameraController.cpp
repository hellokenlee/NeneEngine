#include "PyCameraController.h"
#include "../NeneEngine/CameraController.h"

namespace py = pybind11;

void BindCameraController(py::module& mod)
{
	
	auto cls = py::class_<CameraController, std::shared_ptr<CameraController>>(mod, "CameraController")
		.def(py::init(py::overload_cast<>(&CameraController::Create)))
		.def("update", &CameraController::Update)
		.def("get_camera", &CameraController::GetCamera)
		.def_readwrite("speed", &CameraController::m_speed)
		.def_readwrite("sensitivity", &CameraController::m_sensitivity)
		;
}
