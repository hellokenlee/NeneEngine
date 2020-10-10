/*Copyright reserved by KenLee@2019 hellokenlee@163.com*/
#ifndef DEMO_FXAA_HPP
#define DEMO_FXAA_HPP

#include "DemoBase.h"
#include "../Source/Debug.h"
#include "../Source/Nene.h"

using namespace std;

#define NENE_FXAA_CB_SLOT 2

struct FXAACBDS {
	//
	NNVec4 winSize;
};


class DemoFXAA : public DemoBase {
public:
	DemoFXAA() : DemoBase("FXAA") {}
	virtual ~DemoFXAA() {};
	//
	virtual void onInit() {
		//
		mpCube = Geometry::createCube(CLOCK_WISE);
		mpShaderCommon = Shader::create("Resource/Shader/GLSL/Texture.vert", "Resource/Shader/GLSL/Texture.frag", POSITION_NORMAL_TEXTURE);
		mpTextureDiiffse = Texture2D::create("Resource/Texture/Pure.png");
		//
		auto winWidth = Utils::getWindowWidth();
		auto winHeight = Utils::getWindowHeight();
		mpQuad = Geometry::createQuad(CLOCK_WISE);
		mpScreenRT = RenderTarget::create(winWidth, winHeight, 1);
		mpShaderScreen = Shader::create("Resource/Shader/GLSL/FXAA.vert", "Resource/Shader/GLSL/FXAA.frag", POSITION_NORMAL_TEXTURE);
		//
		mCB.data.winSize = NNVec4(winWidth, winHeight, 1.0 / (float)(winWidth), 1.0 / (float)(winHeight));
	}
	virtual void onLoop(shared_ptr<Camera> camera) {
		//
		mCB.update(NENE_FXAA_CB_SLOT);
		//
		mpScreenRT->begin();
		{
			Utils::clear();
			mpTextureDiiffse->use(0);
			mpCube->draw(mpShaderCommon);
		}
		mpScreenRT->end();
		//
		Utils::clear();
		mpScreenRT->getColorTex(0)->use(0);
		mpQuad->draw(mpShaderScreen);
	}
	virtual void onEnter() {
		printf("FXAA Enter\n");
		Utils::clearColor(1.0f, 0.0f, 0.0f);
	};
	virtual void onLeave() {
		printf("FXAA Leave\n");
	};
protected:
	shared_ptr<Shape> mpCube;
	shared_ptr<Shader> mpShaderCommon;
	shared_ptr<Texture> mpTextureDiiffse;
	//
	shared_ptr<Shape> mpQuad;
	shared_ptr<Shader> mpShaderScreen;
	shared_ptr<RenderTarget> mpScreenRT;
	//
	ConstantBuffer<FXAACBDS> mCB;
};

#endif // DEMO_FXAA_HPP