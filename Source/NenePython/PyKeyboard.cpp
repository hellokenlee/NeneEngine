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
		.def_readonly("on_press", &Keyboard::m_on_press)
		.def_readonly("on_repeat", &Keyboard::m_on_repeat)
		.def_readonly("on_release", &Keyboard::m_on_release)
		;
}
