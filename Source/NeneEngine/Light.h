/*Copyright reserved by KenLee@2019 hellokenlee@163.com*/
#ifndef LIGHT_H
#define LIGHT_H

#include <memory>
#include "Types.h"

struct LightProperty
{

};

class Light 
{
public:
	static std::shared_ptr<Light> Create();

	inline NNLightType GetType() { return m_type; };
	inline void SetType(const NNLightType t) { m_type = t; };

	inline NNVec3 GetPosition() { return m_position; }
	inline void SetPosition(const NNVec3 pos) { m_position = pos; }

	inline NNVec3 GetDirection() { return m_direction; }
	inline void SetDirection(const NNVec3 dir) { m_direction = dir; }
protected:
	NNLightType m_type;
	NNFloat m_range;
	NNVec3 m_position;
	NNVec3 m_direction;
	NNVec4 m_attenuation;
	NNVec3 m_color_ambient;
	NNVec3 m_color_diffuse;
	NNVec3 m_color_specular;
protected:
	Light();
	Light(const Light& rhs) = delete;
	Light& operator=(const Light& rhs) = delete;
};

#endif // LIGHT_H
