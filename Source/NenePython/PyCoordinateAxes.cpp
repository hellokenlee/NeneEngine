#include "PyCoordinateAxes.h"
#include "../NeneEngine/CoordinateAxes.h"

namespace py = pybind11;

void BindCoordinateAxes(py::module& mod)
{
	auto cls = py::class_<CoordinateAxes, std::shared_ptr<CoordinateAxes>>(mod, "CoordinateAxes")
		.def(py::init(&CoordinateAxes::Create))
		.def("draw", &CoordinateAxes::Draw, py::arg("pShader") = std::shared_ptr<Shader>(nullptr), py::arg("pCamera") = std::shared_ptr<Camera>(nullptr))
		.def_readwrite("show_axes", &CoordinateAxes::mIsShowAxes)
		.def_readwrite("show_grids", &CoordinateAxes::mIsShowGrids)
		;
}
