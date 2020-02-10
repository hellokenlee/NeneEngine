#include "PyUtils.h"
#include "../NeneEngine/Utils.h"

void BindUtils(pybind11::module& mod)
{
	auto cls = pybind11::class_<Utils, std::shared_ptr<Utils>>(mod, "Utils")
		.def_static("init", &Utils::init)
		.def_static("clear", &Utils::clear)
		.def_static("Update", &Utils::Update)
		.def_static("terminate", &Utils::terminate)
		.def_static("clear_color", &Utils::clearColor)
		.def_static("swap_buffers", &Utils::swapBuffers)
		.def_static("window_should_close", &Utils::windowShouldClose)
		.def_static("set_window_should_close", &Utils::setWindowShouldClose)
	;
}
