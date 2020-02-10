#include "PyEvent.h"
#include "../NeneEngine/Event.h"

void BindEvent(pybind11::module& mod)
{
	auto baseclass = pybind11::class_<BaseEvent, std::shared_ptr<BaseEvent>>(mod, "Event")
		.def_readonly("code", &BaseEvent::mCode)
		;

	auto subclass1 = pybind11::class_<KeyboardEvent, std::shared_ptr<KeyboardEvent>>(mod, "KeyPressEvent", baseclass)
		.def_readonly("key", &KeyboardEvent::mKey)
		;

	auto subclass2 = pybind11::class_<MouseMoveEvent, std::shared_ptr<MouseMoveEvent>>(mod, "MouseMoveEvent", baseclass)
		.def_readonly("dx", &MouseMoveEvent::mdx)
		.def_readonly("dy", &MouseMoveEvent::mdy)
		;

	auto subclass3 = pybind11::class_<MouseScrollEvent, std::shared_ptr<MouseScrollEvent>>(mod, "MouseScrollEvent", baseclass)
		.def_readonly("dx", &MouseScrollEvent::mdx)
		.def_readonly("dy", &MouseScrollEvent::mdy)
		;
}
