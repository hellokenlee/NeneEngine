#include "PyShape.h"
#include "../NeneEngine/Shape.h"
#include "../NeneEngine/Model.h"
#include "../NeneEngine/Drawable.h"

namespace py = pybind11;

void BindShape(py::module& mod)
{
	auto drawable = py::class_<Drawable, std::shared_ptr<Drawable>>(mod, "Drawable")
		.def("move_to", &Drawable::moveTo)
		.def("rotate_x", &Drawable::rotateX)
		.def("rotate_y", &Drawable::rotateY)
		.def("rotate_z", &Drawable::rotateZ)
		.def("scale_to", py::overload_cast<const NNVec3&>(&Drawable::scaleTo))
		.def("scale_to", py::overload_cast<const NNFloat&>(&Drawable::scaleTo))
		.def("get_model_mat", &Drawable::getModelMat)
		.def("set_model_mat", &Drawable::setModelMat)
		;
	
	auto shape = py::class_<Shape, std::shared_ptr<Shape>>(mod, "Shape", drawable)
		.def("Draw", &Shape::Draw, py::arg("pShader") = std::shared_ptr<Shader>(nullptr), py::arg("pCamera") = std::shared_ptr<Camera>(nullptr))
		.def("draw_instanced", &Shape::DrawInstanced)
		.def("set_draw_mode", &Shape::SetDrawMode)
		;

	auto model = py::class_<Model, std::shared_ptr <Model>>(mod, "Model", shape)
		.def(py::init(&Model::Create))
		;
}
