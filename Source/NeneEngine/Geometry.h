/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Shape.h"

//
//    Geometry: 
//
class Geometry 
{
public:
	// 创建一个UV球体
	static std::shared_ptr<Shape> CreateSphereUV(const NNUInt& latLines = 3, const NNUInt& longLines = 3, NNVertexOrder vo = COUNTER_CLOCK_WISE);
	// 创建一个ICO球体
	static std::shared_ptr<Shape> CreateSphereIco(const NNUInt& level = 3, NNVertexOrder vo = COUNTER_CLOCK_WISE);
	// 创建一个立方体
	static std::shared_ptr<Shape> CreateCube(NNVertexOrder vo = COUNTER_CLOCK_WISE);
	// 创建一个四边形
	static std::shared_ptr<Shape> CreateQuad(NNVertexOrder vo = COUNTER_CLOCK_WISE);
	// 创建一个圆锥
	static std::shared_ptr<Shape> CreateCone(NNVertexOrder vo = COUNTER_CLOCK_WISE);
	// 创建一个圆环
	static std::shared_ptr<Shape> CreateTorus(NNVertexOrder vo = COUNTER_CLOCK_WISE);
	// 生成法向量
	static void CalcNormals(std::vector<NNFloat> &vertices, std::vector<NNUInt> &indices);
	//
	static void InvertIndexOrder(NNUInt *indices, NNUInt iNum);
};

#endif // GEOMETRY_H