/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef COORDINATE_AXES_H
#define COORDINATE_AXES_H

#include "Types.h"
#include "Shape.h"

//
//	坐标轴和网格
//
class CoordinateAxes : public std::enable_shared_from_this<CoordinateAxes> {
public:
	//
	static std::shared_ptr<CoordinateAxes> Create(const NNFloat& lineLen, const NNFloat& lineGap);
	// 绘制函数
	void Draw(const std::shared_ptr<Shader> pShader = nullptr, const std::shared_ptr<Camera> pCamera = nullptr);
public:
	// 是否显示网格
	bool mIsShowGrids;
	// 是否显示坐标轴
	bool mIsShowAxes;
private:
	// 线段长度
	NNFloat mLineLen;
	// 线段间距
	NNFloat mLineGap;
	// 
	std::shared_ptr<Shape> mpGrids, mpAxes;
	std::shared_ptr<Shader> mpShader;
private:
	//
	CoordinateAxes();
	CoordinateAxes(const CoordinateAxes& rhs);
	CoordinateAxes& operator=(const CoordinateAxes& rhs);
	//
	void initAxesData();
	void initGridsData();
};

#endif // COORDINATE_AXES_H
