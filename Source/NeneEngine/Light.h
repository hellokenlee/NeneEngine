/*Copyright reserved by KenLee@2019 hellokenlee@163.com*/
#ifndef LIGHT_H
#define LIGHT_H

#include <memory>
#include "Types.h"

class Light {
public:
	static std::shared_ptr<Light> Create();
public:
	NNLightType mType;
	NNFloat mRange;
	NNVec3 mPosition;
	NNVec3 mDirection;
	NNVec4 mAttenuation;
	NNVec3 mColorAmbient;
	NNVec3 mColorDiffuse;
	NNVec3 mColorSpecular;
private:
	Light();
	Light(const Light& rhs);
	Light& operator=(const Light& rhs);
};

#endif // SHADOW_MAP_H