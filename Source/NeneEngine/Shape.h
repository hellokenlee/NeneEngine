/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef SHAPE_H
#define SHAPE_H

#include "Drawable.h"
#include <vector>
#include <memory>

//
//    Shape: Collection of vertices to Draw
//

class Shape : public Drawable {
public:
	// 静态构造方法
	static std::shared_ptr<Shape> Create(NNFloat* pVertices, NNUInt vArrayLen, NNVertexFormat vf);
	static std::shared_ptr<Shape> Create(std::vector<NNFloat> vertices, NNVertexFormat vf);
	static std::shared_ptr<Shape> Create(NNFloat* pVertices, NNUInt vArrayLen, NNUInt* pIndices, NNUInt iArrayLen, NNVertexFormat vf);
	static std::shared_ptr<Shape> Create(std::vector<NNFloat> vertices, std::vector<NNUInt> indices, NNVertexFormat vf);
	// 析构函数
	virtual ~Shape();
	// 绘制函数
	virtual void Draw(const std::shared_ptr<Shader> pShader = nullptr,
		const std::shared_ptr<Camera> pCamera = nullptr) override;
	virtual void DrawInstanced(const std::shared_ptr<Shader> pShader = nullptr,
		const std::shared_ptr<Camera> pCamera = nullptr) override;
	// 设置绘制模式
	void SetDrawMode(NNDrawMode newMode);
protected:
	// 顶点数，索引数
	NNUInt mVertexNum, mIndexNum;
	// 顶点格式
	NNVertexFormat mVertexFormat;
	// 绘制模式
	NNDrawMode mDrawMode;
	// 顶点缓冲
#if defined NENE_GL
	NNUInt mVAO, mVBO, mEBO;
#elif defined NENE_DX
	ID3D11Buffer *mpVertexBuffer, *mpIndexBuffer;
	NNUInt mPerVertexSize, mOffset;
#else
	#error Please define NENE_GL or NENE_DX to select which Graphic-API you want to Use!
#endif

private:
	Shape();
	Shape(const Shape& rhs);
	Shape& operator=(const Shape& rhs);
};

#endif // SHAPE_H