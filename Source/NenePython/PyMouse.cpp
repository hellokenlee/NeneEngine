#include <stl.h>
#include <functional.h>
#include "PyMouse.h"
#include "../NeneEngine/Mouse.h"

namespace py = pybind11;

void BindMouse(py::module& mod)
{
	auto cls_nene = py::class_<Mouse>(mod, "Mouse")
		.def_static("instance", &Mouse::Instance, py::return_value_policy::reference)
		.def_readonly("on_move", &Mouse::onMove)
		.def_readonly("on_click", &Mouse::onClick)
		.def_readonly("on_scroll", &Mouse::onScroll)
		;
}
