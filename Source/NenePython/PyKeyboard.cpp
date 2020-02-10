#include <stl.h>
#include <functional.h>
#include "PyKeyboard.h"
#include "../NeneEngine/Keyboard.h"

namespace py = pybind11;

void BindKeyboard(py::module& mod)
{
	auto cls_nene = py::class_<Keyboard>(mod, "Keyboard")
		.def_static("instance", &Keyboard::Instance, py::return_value_policy::reference)
		.def("get_key", &Keyboard::GetKey)
		.def("on_press", &Keyboard::OnPress)
		.def("on_repeat", &Keyboard::OnRepeat)
		.def("on_release", &Keyboard::OnRelease)
		;
}
