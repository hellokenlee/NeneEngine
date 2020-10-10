#include "PyCamera.h"
#include "../NeneEngine/Camera.h"

void BindCamera(pybind11::module& mod)
{
	auto cls = pybind11::class_<Camera, std::shared_ptr<Camera>>(mod, "Camera")
		.def(pybind11::init<>())
		.def("use", &Camera::Use)
	;
}
