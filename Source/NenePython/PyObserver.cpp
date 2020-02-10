#include <stl.h>
#include <functional.h>
#include "PyObserver.h"
#include "../NeneEngine/Observer.h"
#include "../NeneEngine/Observable.h"

namespace py = pybind11;

void BindObserver(py::module& mod)
{
	auto cls_osv = py::class_<Observer, std::shared_ptr<Observer>>(mod, "Observer")
		.def("on_notify", &Observer::OnNotify)
		;

	auto cls_osa = py::class_<Observable, std::shared_ptr<Observable>>(mod, "Obserable")
		.def("Notify", &Observable::Notify)
		.def("add_observer", &Observable::AddObserver)
		.def("del_observer", &Observable::DelObserver)
		.def("add_callback", &Observable::addCallbackFunc)
		.def("del_callback", &Observable::delCallbackFunc)
		;
}
