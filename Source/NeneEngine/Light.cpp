/*Copyright reserved by KenLee@2019 hellokenlee@163.com*/

#include "Light.h"

Light::Light() :
	mType(DIRECTIONAL_LIGHT), mRange(1000.0f), mPosition(0.0f), mDirection(-1.0f),
	mAttenuation(0.0f), mColorAmbient(1.0f), mColorDiffuse(1.0f), mColorSpecular(1.0f)
	{}

std::shared_ptr<Light> Light::Create()
{
	return std::shared_ptr<Light>(new Light);
}
