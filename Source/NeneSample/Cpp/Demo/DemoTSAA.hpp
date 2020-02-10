/*Copyright reserved by KenLee@2019 hellokenlee@163.com*/
#ifndef DEMO_TSAA_HPP
#define DEMO_TSAA_HPP

#include "DemoBase.h"
#include "../Source/Debug.h"
#include "../Source/Nene.h"

using namespace std;

#define TSAA_SAMPLE_NUM 8

#define NENE_FXAA_CB_SLOT 2

const float HALTON_2_3_X[] = { 1.0f / 2.0f, 1.0f / 4.0f, 3.0f / 4.0f, 1.0f / 8.0f, 5.0f / 8.0f, 3.0f / 8.0f, 7.0f / 8.0f, 1.0f / 16.0f, 9.0f / 16.0f };
const float HALTON_2_3_Y[] = { 1.0f / 3.0f, 2.0f / 3.0f, 1.0f / 9.0f, 4.0f / 9.0f, 7.0f / 9.0f, 2.0f / 9.0f, 5.0f / 9.0f, 8.0f /  9.0f, 1.0f / 27.0f };

struct TSAACBDS {
	// 当前帧的矩阵的逆
	NNMat4 inversedView;
	NNMat4 inversedProj;
	// 所有帧的矩阵记录
	NNMat4 views[TSAA_SAMPLE_NUM];
	NNMat4 projs[TSAA_SAMPLE_NUM];
	// 当前帧下标
	NNUInt currTexIdx;
};

class DemoTSAA : public DemoBase {
public:
	DemoTSAA() : DemoBase("TSAA") {}
	virtual ~DemoTSAA() {};
	//
	virtual void onInit() {
		// 场景
		mpCube = Geometry::createCube(CLOCK_WISE);
		mpShaderCommon = Shader::create("Resource/Shader/GLSL/Texture.vert", "Resource/Shader/GLSL/Texture.frag", POSITION_NORMAL_TEXTURE);
		mpTextureDiiffse = Texture2D::create("Resource/Texture/Pure.png");
		// 屏幕四边形和着色器
		mpQuad = Geometry::createQuad(CLOCK_WISE);
		mpShaderScreen = Shader::create("Resource/Shader/GLSL/TSAA.vert", "Resource/Shader/GLSL/TSAA.frag", POSITION_NORMAL_TEXTURE);
		// RTs
		auto winWidth = Utils::getWindowWidth();
		auto winHeight = Utils::getWindowHeight();
		//
		for (size_t i = 0; i < TSAA_SAMPLE_NUM; ++i) {
			//
			mpScreenRTs.emplace_back(shared_ptr<RenderTarget>(RenderTarget::create(winWidth, winHeight, 1)));
			mpScreenRTs[i]->getColorTex(0)->mode(AS_COLOR);
			mpScreenRTs[i]->getDepthStencilTex()->mode(AS_DEPTH);
		}
		//
		mCurrentIdx = 0;
	}
	virtual void onLoop(shared_ptr<Camera> camera) {
		// 应用抖动
		static float origin20 = camera->getProjMat()[2][0];
		static float origin21 = camera->getProjMat()[2][1];
		camera->getProjMat()[2][0] = origin20 + ((HALTON_2_3_X[mCurrentIdx] * 2.0f - 1.0f) / Utils::getWindowWidth());
		camera->getProjMat()[2][1] = origin21 + ((HALTON_2_3_Y[mCurrentIdx] * 2.0f - 1.0f) / Utils::getWindowHeight());
		// 更新内置UBO
		camera->use();
		NeneCB::getInstance().PerFrame.update(NENE_PER_FRAME_CB_SLOT);
	
		// 记录各个矩阵的值
		camera->getProjMat()[2][0] = origin20;
		camera->getProjMat()[2][1] = origin21;
		mCB.data.inversedView = camera->getInversedViewMat();
		mCB.data.inversedProj = camera->getInversedProjMat();
		mCB.data.views[mCurrentIdx] = camera->getViewMat();
		mCB.data.projs[mCurrentIdx] = camera->getProjMat();
		mCB.data.currTexIdx = mCurrentIdx;
		// 更新自定义UBO
		mCB.update(NENE_FXAA_CB_SLOT);

		// 使用该帧的RT
		mpScreenRTs[mCurrentIdx]->begin();
		{
			// 场景绘制
			Utils::clear();
			mpTextureDiiffse->use(0);
			mpCube->draw(mpShaderCommon);
		}
		mpScreenRTs[mCurrentIdx]->end();

		// 绘制平面
		Utils::clear();
		for (size_t i = 0; i < TSAA_SAMPLE_NUM; ++i) {
			mpScreenRTs[i]->getColorTex(0)->use(i);
		}
		for (size_t i = 0; i < TSAA_SAMPLE_NUM; ++i) {
			mpScreenRTs[i]->getDepthStencilTex()->use(TSAA_SAMPLE_NUM + i);
		}
		mpQuad->draw(mpShaderScreen);
		// 更新下标
		++mCurrentIdx;
		mCurrentIdx %= TSAA_SAMPLE_NUM;
	}
	virtual void onEnter() {
		printf("TSAA Enter\n");
		Utils::clearColor(1.0f, 0.0f, 0.0f);
	};
	virtual void onLeave() {
		printf("TSAA Leave\n");
	};
protected:
	shared_ptr<Shape> mpCube;
	shared_ptr<Shader> mpShaderCommon;
	shared_ptr<Texture> mpTextureDiiffse;
	//
	size_t mCurrentIdx;
	vector<shared_ptr<RenderTarget>> mpScreenRTs;
	shared_ptr<Shape> mpQuad;
	shared_ptr<Shader> mpShaderScreen;
	//
	ConstantBuffer<TSAACBDS> mCB;
};

#endif // DEMO_TSAA_HPP