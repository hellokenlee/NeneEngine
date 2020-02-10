/*Copyright reserved by KenLee@2019 hellokenlee@163.com*/

#include "Light.h"

std::shared_ptr<Light> Light::Create()
{
	return std::shared_ptr<Light>(new Light);
}
