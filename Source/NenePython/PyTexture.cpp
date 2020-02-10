#include "PyTexture.h"
#include "../NeneEngine/Texture.h"
#include "../NeneEngine/Texture2D.h"
#include "../NeneEngine/Texture3D.h"
#include "../NeneEngine/TextureCube.h"

void BindTexture(pybind11::module& mod)
{
	auto tex = pybind11::class_<Texture, std::shared_ptr<Texture>>(mod, "Texture")
		.def("Use", &Texture::Use);

	auto tex2d = pybind11::class_<Texture2D, std::shared_ptr<Texture2D>>(mod, "Texture2D", tex)
		.def(pybind11::init<>(&Texture2D::Create))
		.def("mode", &Texture2D::mode)
		;
	 
	auto texcube = pybind11::class_<TextureCube, std::shared_ptr<TextureCube>>(mod, "TextureCbue", tex)
		.def(pybind11::init<>(&TextureCube::Create))
		;

}
