/*Copyright reserved by KenLee@2019 hellokenlee@163.com*/
#ifndef LIGHT_H
#define LIGHT_H

#include <memory>
#include "Types.h"

//
struct LightCBDS
{
	//
	NNFloat ltype;
	NNFloat range;
	NNFloat _pad0;
	NNFloat _pad1;
	//
	NNVec4 color;
	NNVec4 position;
	NNVec4 direction;
	NNFloat attenuation;
};

#endif // LIGHT_H
