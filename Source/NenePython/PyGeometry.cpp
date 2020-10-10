#include "PyGeometry.h"
#include "../NeneEngine/Geometry.h"

void BindGeometry(pybind11::module& mod)
{
	auto cls = pybind11::class_<Geometry, std::shared_ptr<Geometry>>(mod, "Geometry")
		.def_static("create_quad", &Geometry::CreateQuad)
		.def_static("create_cube", &Geometry::CreateCube)
		.def_static("create_sphere_uv", &Geometry::CreateSphereUV)
		.def_static("create_sphere_ico", &Geometry::CreateSphereIco)
	;
}
