/*Copyright reserved by KenLee@2019 hellokenlee@163.com*/

#include "Light.h"

Light::Light() :
	m_type(DIRECTIONAL_LIGHT), m_range(1000.0f), m_position(0.0f), m_direction(-1.0f),
	m_attenuation(0.0f), m_color_ambient(1.0f), m_color_diffuse(1.0f), m_color_specular(1.0f)
	{}

std::shared_ptr<Light> Light::Create()
{
	return std::shared_ptr<Light>(new Light);
}
