#include "PyUtils.h"
#include "../NeneEngine/Utils.h"

void BindUtils(pybind11::module& mod)
{
	auto cls = pybind11::class_<Utils, std::shared_ptr<Utils>>(mod, "Utils")
		.def_static("init", &Utils::Init)
		.def_static("clear", &Utils::Clear)
		.def_static("update", &Utils::Update)
		.def_static("terminate", &Utils::Terminate)
		.def_static("clear_color", &Utils::ClearColor)
		.def_static("swap_buffers", &Utils::SwapBuffers)
		.def_static("window_should_close", &Utils::WindowShouldClose)
		.def_static("set_window_should_close", &Utils::SetWindowShouldClose)
	;
}
