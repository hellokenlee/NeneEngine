/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#include "Drawable.h"

Drawable::Drawable() {
	mModelMat = NNMat4Identity;
	mTranslation = NNVec3(0.0f, 0.0f, 0.0f);
	mTranslationMat = NNMat4Identity;
	mRotation = NNVec3(0.0f, 0.0f, 0.0f);
	mRotationMat = NNMat4Identity;
	mScale = NNVec3(0.0f, 0.0f, 0.0f);
	mScaleMat = NNMat4Identity;
}

Drawable::~Drawable() {
	;
}

void Drawable::MoveTo(const NNVec3& position) {
	mTranslation = position;
	mTranslationMat = NNCreateTranslation(mTranslation);
	mModelMat = mTranslationMat * mRotationMat * mScaleMat;
}

void Drawable::ScaleTo(const NNVec3& scale) {
	mScale = scale;
	mScaleMat = NNCreateScale(mScale);
	mModelMat = mTranslationMat * mRotationMat * mScaleMat;
}

void Drawable::ScaleTo(const NNFloat& scale) {
	mScale = NNVec3(scale);
	mScaleMat = NNCreateScale(mScale);
	mModelMat = mTranslationMat * mRotationMat * mScaleMat;
}

void Drawable::RotateX(const NNFloat& radians) {
	mRotation.x = radians;
	mRotationMat = NNMat4Identity;
	mRotationMat = NNCreateRotationX(mRotationMat, mRotation.x);
	mRotationMat = NNCreateRotationY(mRotationMat, mRotation.y);
	mRotationMat = NNCreateRotationZ(mRotationMat, mRotation.z);
	mModelMat = mTranslationMat * mRotationMat * mScaleMat;
}

void Drawable::RotateY(const NNFloat& radians) {
	mRotation.y = radians;
	mRotationMat = NNMat4Identity;
	mRotationMat = NNCreateRotationX(mRotationMat, mRotation.x);
	mRotationMat = NNCreateRotationY(mRotationMat, mRotation.y);
	mRotationMat = NNCreateRotationZ(mRotationMat, mRotation.z);
	mModelMat = mTranslationMat * mRotationMat * mScaleMat;
}

void Drawable::RotateZ(const NNFloat& radians) {
	mRotation.z = radians;
	mRotationMat = NNMat4Identity;
	mRotationMat = NNCreateRotationX(mRotationMat, mRotation.x);
	mRotationMat = NNCreateRotationY(mRotationMat, mRotation.y);
	mRotationMat = NNCreateRotationZ(mRotationMat, mRotation.z);
	mModelMat = mTranslationMat * mRotationMat * mScaleMat;
}

const NNMat4& Drawable::GetModelMat() {
	return mModelMat;
}

void Drawable::SetModelMat(const NNMat4& model) {
	mModelMat = model;
}
