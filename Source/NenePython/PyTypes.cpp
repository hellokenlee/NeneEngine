#include "PyTypes.h"
#include "../NeneEngine/Types.h"
#include "../NeneEngine/NeneCB.h"

enum NNKey
{
	NN_KEY_ESCAPE = NNKeyMap(ESCAPE),
	
	NN_KEY_F1 = NNKeyMap(F1),
	NN_KEY_F2 = NNKeyMap(F2),
	NN_KEY_F3 = NNKeyMap(F3),
	NN_KEY_F4 = NNKeyMap(F4),
	NN_KEY_F5 = NNKeyMap(F5),
	NN_KEY_F6 = NNKeyMap(F6),
	NN_KEY_F7 = NNKeyMap(F7),
	NN_KEY_F8 = NNKeyMap(F8),
	NN_KEY_F9 = NNKeyMap(F9),
	NN_KEY_F10 = NNKeyMap(F10),
	NN_KEY_F11 = NNKeyMap(F11),
	NN_KEY_F12 = NNKeyMap(F12),

	NN_KEY_PRINT_SCREEN = NNKeyMap(PRINT_SCREEN),
	NN_KEY_SCROLL_LOCK = NNKeyMap(SCROLL_LOCK),
	NN_KEY_PAUSE = NNKeyMap(PAUSE),
	NN_KEY_APOSTROPHE = NNKeyMap(APOSTROPHE),
	NN_KEY_COMMA = NNKeyMap(COMMA),
	NN_KEY_MINUS = NNKeyMap(MINUS),
	NN_KEY_PERIOD = NNKeyMap(PERIOD),
	NN_KEY_SLASH = NNKeyMap(SLASH),
};

void BindTypes(pybind11::module& mod)
{	
	auto vec3 = pybind11::class_<NNVec3>(mod, "Vec3")
		.def(pybind11::init<>())
		.def(pybind11::init<float>())
		.def(pybind11::init<float, float, float>())
		.def_readwrite("x", &NNVec3::x)
		.def_readwrite("y", &NNVec3::y)
		.def_readwrite("z", &NNVec3::z)
		;
	
	pybind11::enum_<NNVertexFormat>(mod, "VertexFormat")
		.value("UNDEFINE", NNVertexFormat::UNDEFINE)
		.value("POSITION", NNVertexFormat::POSITION)
		.value("POSITION_TEXTURE", NNVertexFormat::POSITION_TEXTURE)
		.value("POSITION_NORMAL", NNVertexFormat::POSITION_NORMAL)
		.value("POSITION_NORMAL_TEXTURE", NNVertexFormat::POSITION_NORMAL_TEXTURE)
		.export_values()
		;

	pybind11::enum_<NNTextureMode>(mod, "TextureMode")
		.value("AS_COLOR", NNTextureMode::AS_COLOR)
		.value("AS_DEPTH", NNTextureMode::AS_DEPTH)
		.value("AS_STENCIL", NNTextureMode::AS_STENCIL)
		.export_values()
		;

	pybind11::enum_<NNTextureType>(mod, "TextureType")
		.value("DIFFUSE", NNTextureType::DIFFUSE)
		.value("SPECULAR", NNTextureType::SPECULAR)
		.value("REFLECT", NNTextureType::REFLECT)
		.value("NORMAL", NNTextureType::NORMAL)
		.value("ROUGHNESS", NNTextureType::ROUGHNESS)
		.value("HEIGHT", NNTextureType::HEIGHT)
		.value("AMBIENT_OCCLUSION", NNTextureType::AMBIENT_OCCLUSION)
		.value("PBR_ALBEDO", NNTextureType::PBR_ALBEDO)
		.value("PBR_METALNESS", NNTextureType::PBR_METALNESS)
		.value("PBR_ROUGHNESS", NNTextureType::PBR_ROUGHNESS)
		.value("PBR_NORMAL", NNTextureType::PBR_NORMAL)
		.export_values()
		;

	pybind11::enum_<NNShaderType>(mod, "ShaderType")
		.value("VERTEX_SHADER", NNShaderType::VERTEX_SHADER)
		.value("FRAGMENT_SHADER", NNShaderType::FRAGMENT_SHADER)
		.value("GEOMETRY_SHADER", NNShaderType::GEOMETRY_SHADER)
		.value("TESS_CONTROL_SHADER", NNShaderType::TESS_CONTROL_SHADER)
		.value("TESS_EVALUATE_SHADER", NNShaderType::TESS_EVALUATE_SHADER)
		.export_values()
		;


	pybind11::enum_<NNVertexOrder>(mod, "VertexOrder")
		.value("CLOCK_WISE", NNVertexOrder::CLOCK_WISE)
		.value("COUNTER_CLOCK_WISE", NNVertexOrder::COUNTER_CLOCK_WISE)
		.export_values()
		;

	pybind11::enum_<NNConstantBufferSlot>(mod, "ConstantBufferSlot")
		.value("PER_FRAME_SLOT", NNConstantBufferSlot::PER_FRAME_SLOT)
		.value("PER_OBJECT_SLOT", NNConstantBufferSlot::PER_OBJECT_SLOT)
		.export_values()
		;

	pybind11::enum_<NNEventCode>(mod, "EventCode")
		.value("UNDEFINED", NNEventCode::UNDEFINED)
		.value("ON_KEY_PRESS", NNEventCode::ON_KEY_PRESS)
		.value("ON_KEY_RELEASE", NNEventCode::ON_KEY_RELEASE)
		.value("ON_KEY_REPEAT", NNEventCode::ON_KEY_REPEAT)
		.value("ON_MOUSE_MOVE", NNEventCode::ON_MOUSE_MOVE)
		.value("ON_MOUSE_SCROLL", NNEventCode::ON_MOUSE_SCROLL)
		.export_values()
		;

	pybind11::enum_<NNKey>(mod, "Key")
		.value("ESCAPE", NNKey::NN_KEY_ESCAPE)

		.value("F1", NNKey::NN_KEY_F1)
		.value("F2", NNKey::NN_KEY_F2)
		.value("F3", NNKey::NN_KEY_F3)
		.value("F4", NNKey::NN_KEY_F4)
		.value("F5", NNKey::NN_KEY_F5)
		.value("F6", NNKey::NN_KEY_F6)
		.value("F7", NNKey::NN_KEY_F7)
		.value("F8", NNKey::NN_KEY_F8)
		.value("F9", NNKey::NN_KEY_F9)
		.value("F10", NNKey::NN_KEY_F10)
		.value("F11", NNKey::NN_KEY_F11)
		.value("F12", NNKey::NN_KEY_F12)

		.value("PRINT_SCREEN", NNKey::NN_KEY_PRINT_SCREEN)
		.value("SCROLL_LOCK", NNKey::NN_KEY_SCROLL_LOCK)
		.value("PAUSE", NNKey::NN_KEY_PAUSE)
		.value("APOSTROPHE", NNKey::NN_KEY_APOSTROPHE)
		.value("COMMA", NNKey::NN_KEY_COMMA)
		.value("MINUS", NNKey::NN_KEY_MINUS)
		.value("PERIOD", NNKey::NN_KEY_PERIOD)
		.value("SLASH", NNKey::NN_KEY_SLASH)
		.export_values()
		;
}
