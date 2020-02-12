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
	static std::shared_ptr<ShadowMap> Create(const NNFloat& radius, const NNUInt& width, const NNUInt& height);
	//
	void SetLight(std::shared_ptr<Light> shadowlight);
	void SetRadius(const NNFloat rad);
	void SetDistance(const NNFloat dis);
	//
	virtual void Use(const NNUInt& slot = 0) override;
	virtual void Begin() override;
	virtual void End() override;
protected:
	// 阴影贴图的正方形半径
	NNFloat m_radius;
	// 阴影贴图到摄像机的距离
	NNFloat m_distance;
	NNMat4 m_light_view_mat;
	NNMat4 m_light_proj_mat;
	std::shared_ptr<Light> m_light;
private:
	ShadowMap() = delete;
	ShadowMap(const ShadowMap& rhs) = delete;
	ShadowMap& operator=(const ShadowMap& rhs) = delete;
	ShadowMap(const NNFloat& volume, const NNUInt& width, const NNUInt& height);
};

#endif // SHADOW_MAP_H