/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef INSTANCE_H
#define INSTANCE_H

#include "Drawable.h"
#include <vector>

//
//    Instance: 
//
template<typename T>
class Instance {
public:
	std::shared_ptr<Instance> Create(std::shared_ptr<Drawable> shape, std::vector<T> &data);
	~Instance();
	void Draw(Shader *pShader, Camera *pCamera);
private:
	//
	Instance();
	Instance(const Instance &rhs);
	Instance& operator=(const Instance& rhs);
	//
	NNUInt mOffset;
	NNUInt mInstancesNum;
	NNUInt mPerInstanceSize;
	//
	std::shared_ptr<Drawable> mpShape;
	//
#if defined NENE_GL
	NNUInt mVBO;
#elif defined NENE_DX
	ID3D11Buffer* pInstanceBuffer;
#else
	#error Please define NENE_GL or NENE_DX to select which Graphic-API you want to Use!
#endif
};

#endif // INSTANCE_H
