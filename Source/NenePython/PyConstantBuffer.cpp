#include "PyConstantBuffer.h"
#include "../NeneEngine/NeneCB.h"

#include "../NeneEngine/ConstantBuffer.h"

namespace py = pybind11;

void BindConstantBuffer(py::module& mod)
{	
	auto cls_pf = py::class_<ConstantBuffer<PerFrameCBDS>>(mod, "__NeneConstantBuffer__PerFrame")
		.def("update", &ConstantBuffer<PerFrameCBDS>::Update)
		;

	auto cls_po = py::class_<ConstantBuffer<PerObjectCBDS>>(mod, "__NeneConstantBuffer__PerObject")
		.def("update", &ConstantBuffer<PerObjectCBDS>::Update)
		;
	
	auto cls_nene = py::class_<NeneCB>(mod, "NeneConstantBuffer")
		.def_static("instance", &NeneCB::Instance, py::return_value_policy::reference)
		.def("per_frame", &NeneCB::PerFrame)
		.def("per_object", &NeneCB::PerObject)
		;
}
