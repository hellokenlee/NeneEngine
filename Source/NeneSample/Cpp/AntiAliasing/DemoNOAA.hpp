/*Copyright reserved by KenLee@2019 hellokenlee@163.com*/
#ifndef DEMO_NOAA_HPP
#define DEMO_NOAA_HPP

#include "DemoBase.h"
#include "../Source/Debug.h"
#include "../Source/Nene.h"

using namespace std;

class DemoNOAA : public DemoBase {
public:
	DemoNOAA() : DemoBase("NOAA") {}
	//
	virtual void onInit() {
		//
		mpCube = Geometry::createCube(CLOCK_WISE);
		mpShaderCommon = Shader::create("Resource/Shader/GLSL/Texture.vert", "Resource/Shader/GLSL/Texture.frag", POSITION_NORMAL_TEXTURE);
		mpTextureDiiffse = Texture2D::create("Resource/Texture/Pure.png");
	}
	virtual void onLoop(shared_ptr<Camera> camera) {
		//
		Utils::clear();
		mpTextureDiiffse->use(0);
		mpCube->draw(mpShaderCommon);
	}
	virtual void onEnter() {
		printf("NOAA Enter\n");
		Utils::clearColor(1.0f, 0.0f, 0.0f);
	};
	virtual void onLeave() {
		printf("NOAA Leave\n");
	};
protected:
	shared_ptr<Shape> mpCube;
	shared_ptr<Shader> mpShaderCommon;
	shared_ptr<Texture> mpTextureDiiffse;

};


#endif // DEMO_NOAA_HPP