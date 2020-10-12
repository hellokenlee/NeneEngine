/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef SHADOW_MAP_H
#define SHADOW_MAP_H

#include "Light.h"
#include "Texture.h"
#include "RenderTarget.h"


#include <vector>
#include <memory>

//
//    ShadowMap: 
//

class ShadowMap : public Texture, RenderTarget {
public:
	//
	static std::shared_ptr<ShadowMap> Create(const NNFloat& volume, const NNUInt& width, const NNUInt& height);
	//
	virtual void Use(const NNUInt& slot = 0) override;
	virtual void Begin() override;
	virtual void End() override;
protected:
	NNFloat m_volume;
	NNMat4 m_light_view_mat;
	NNMat4 m_light_proj_mat;
private:
	ShadowMap() = default;
	ShadowMap(const ShadowMap& rhs);
	ShadowMap& operator=(const ShadowMap& rhs);
};

#endif // SHADOW_MAP_H