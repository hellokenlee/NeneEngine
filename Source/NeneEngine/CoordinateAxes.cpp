/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/

#include "CoordinateAxes.h"

#include "CoordinateAxesShader_DX.inl"

#include "CoordinateAxesShader_GL.inl"

using namespace std;

CoordinateAxes::CoordinateAxes() : mLineLen(0), mLineGap(0), mpAxes(nullptr), mpGrids(nullptr), mpShader(nullptr) {}

shared_ptr<CoordinateAxes> CoordinateAxes::Create(const NNFloat& lineLen, const NNFloat& lineGap) {
	//
	static shared_ptr<Shader> pShader = Shader::createFromSource(vShaderSource, fShaderSource, POSITION_NORMAL);
	//
	CoordinateAxes* ret = new CoordinateAxes();
	ret->mpShader = pShader;
	ret->mLineLen = lineLen;
	ret->mLineGap = lineGap;
	//
	ret->initAxesData();
	ret->initGridsData();
	//
	if (ret->mpAxes != nullptr && ret->mpGrids != nullptr) {
		ret->mpAxes->SetDrawMode(NN_LINE);
		ret->mpGrids->SetDrawMode(NN_LINE);
		return shared_ptr<CoordinateAxes>(ret);
	}
	//
	delete ret;
	//
	return nullptr;
}

void CoordinateAxes::Draw(const shared_ptr<Shader> pShader, const shared_ptr<Camera> pCamera) {
	if (pShader != nullptr) {
		mpAxes->Draw(pShader, pCamera);
		mpGrids->Draw(pShader, pCamera);
	}
	else {
		mpAxes->Draw(mpShader, pCamera);
		mpGrids->Draw(mpShader, pCamera);
	}
	
}

void CoordinateAxes::initAxesData() {
	// 初始化顶点数组
	NNFloat verticesPC[6 * 6];
	//
	for (int i = 0; i < 36; ++i) {
		if (i == 6 || i == 19 || i == 32) {
			verticesPC[i] = mLineLen;
		}
		else if (i == 3 || i == 9 || i == 16 || i == 22 || i == 29 || i == 35) {
			verticesPC[i] = 0.5f;
		}
		else {
			verticesPC[i] = 0.0f;
		}
	}
	//
	mpAxes = Shape::Create(verticesPC, 36, POSITION_NORMAL);
}

void CoordinateAxes::initGridsData() {
	//
	vector<NNFloat> verticesPC((int)((mLineLen / mLineGap + 1) * 2 * 2 * 2 * 6));
	//
	for (size_t i = 0; i < verticesPC.size(); ++i) {
		verticesPC[i] = 0.6f;
	}
	// 顶点序号
	int i = 0;
	// 平行Z轴
	float x = 0;
	while (x < mLineLen) {
		verticesPC[i * 6 + 0] = x;
		verticesPC[i * 6 + 1] = 0;
		verticesPC[i * 6 + 2] = (x == 0 ? 0 : mLineLen);
		i++;
		verticesPC[i * 6 + 0] = x;
		verticesPC[i * 6 + 1] = 0;
		verticesPC[i * 6 + 2] = -mLineLen;
		i++;
		verticesPC[i * 6 + 0] = -x;
		verticesPC[i * 6 + 1] = 0;
		verticesPC[i * 6 + 2] = (x == 0 ? 0 : mLineLen);
		i++;
		verticesPC[i * 6 + 0] = -x;
		verticesPC[i * 6 + 1] = 0;
		verticesPC[i * 6 + 2] = -mLineLen;
		i++;
		x += mLineGap;
	}
	// 平行X轴
	float z = 0;
	while (z < mLineLen) {
		verticesPC[i * 6 + 0] = (z == 0 ? 0 : mLineLen);
		verticesPC[i * 6 + 1] = 0;
		verticesPC[i * 6 + 2] = z;
		i++;
		verticesPC[i * 6 + 0] = -mLineLen;
		verticesPC[i * 6 + 1] = 0;
		verticesPC[i * 6 + 2] = z;
		i++;
		verticesPC[i * 6 + 0] = (z == 0 ? 0 : mLineLen);
		verticesPC[i * 6 + 1] = 0;
		verticesPC[i * 6 + 2] = -z;
		i++;
		verticesPC[i * 6 + 0] = -mLineLen;
		verticesPC[i * 6 + 1] = 0;
		verticesPC[i * 6 + 2] = -z;
		i++;
		z += mLineGap;
	}
	//
	mpGrids = Shape::Create(verticesPC, POSITION_NORMAL);
}

