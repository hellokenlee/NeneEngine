/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef DRAWABLE_H
#define DRAWABLE_H

#include "Shader.h"
#include "Camera.h"

#include <memory>

//
//    Drawable: Abstract Drawable Class
//

class Drawable : public std::enable_shared_from_this<Drawable> {
public:
	// 构造
	Drawable();
	virtual ~Drawable();
	// 绘制
	virtual void Draw(const std::shared_ptr<Shader> pShader = nullptr, 
		const std::shared_ptr<Camera> pCamera = nullptr) = 0;
	// 实例化绘制
	virtual void DrawInstanced(const std::shared_ptr<Shader> pShader = nullptr,
		const std::shared_ptr<Camera> pCamera = nullptr) = 0;
	// 变换函数
	virtual void MoveTo(const NNVec3& position);
	virtual void ScaleTo(const NNVec3& scale);
	virtual void ScaleTo(const NNFloat& scale);
	virtual void RotateX(const NNFloat& radians);
	virtual void RotateY(const NNFloat& radians);
	virtual void RotateZ(const NNFloat& radians);
	// 获取变换矩阵
	const NNMat4& GetModelMat();
	// 更改变换矩阵
	void SetModelMat(const NNMat4& model);
protected:
	// 变换矩阵
	NNMat4 mModelMat;
	// 记录位移
	NNVec3 mTranslation;
	NNMat4 mTranslationMat;
	// 记录旋转
	NNVec3 mRotation;
	NNMat4 mRotationMat;
	// 记录缩放
	NNVec3 mScale;
	NNMat4 mScaleMat;
};


#endif // DRAWABLE_H