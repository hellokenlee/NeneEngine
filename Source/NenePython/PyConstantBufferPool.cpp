#include "PyConstantBufferPool.h"
#include "../NeneEngine/ConstantBufferPool.h"

namespace py = pybind11;

void BindConstantBufferPool(py::module& mod)
{
	auto cls = py::class_<ConstantBufferPool, std::shared_ptr<ConstantBufferPool>>(mod, "ConstantBufferPool")
		.def(py::init<>())
		.def("len", &ConstantBufferPool::len)
		.def("size", &ConstantBufferPool::size)
		.def("update", &ConstantBufferPool::Update)
		.def("write", &ConstantBufferPool::write<float>)
		.def("append", &ConstantBufferPool::append<float>)
		;
}
