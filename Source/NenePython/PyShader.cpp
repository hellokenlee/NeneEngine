#include "PyShader.h"
#include "../NeneEngine/Shader.h"

namespace py = pybind11;

void BindShader(py::module& mod)
{
	auto cls = py::class_<Shader, std::shared_ptr<Shader>>(mod, "Shader")
		.def(py::init(&Shader::Create))
		.def("add_optional_shader", &Shader::addOptionalShader)
		.def("Use", &Shader::Use)
	;
}
