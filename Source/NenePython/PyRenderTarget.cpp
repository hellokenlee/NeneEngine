#include "PyRenderTarget.h"
#include "../NeneEngine/RenderTarget.h"

namespace py = pybind11;

void BindRenderTarget(py::module& mod)
{
	auto shape = py::class_<RenderTarget, std::shared_ptr<RenderTarget>>(mod, "RenderTarget")
		.def(py::init(&RenderTarget::Create))
		.def(py::init(&RenderTarget::CreateMultisample))
		.def("end", &RenderTarget::End)
		.def("begin", &RenderTarget::Begin)
		.def("get_color_texture", &RenderTarget::GetColorTex)
		.def("get_depth_stencil_texture", &RenderTarget::GetDepthStencilTex)
		;
}
