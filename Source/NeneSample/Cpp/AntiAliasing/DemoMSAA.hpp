/*Copyright reserved by KenLee@2019 hellokenlee@163.com*/
#ifndef DEMO_MSAA_HPP
#define DEMO_MSAA_HPP

#include "DemoBase.h"
#include "../Source/Debug.h"
#include "../Source/Nene.h"

using namespace std;

class DemoMSAA : public DemoBase {
public:
	DemoMSAA(): DemoBase("MSAA"), mSamples(4), mFilter(GL_LINEAR) {}
	//
	virtual void onInit() {
		//
		glEnable(GL_MULTISAMPLE);
		//
		mpCube = Geometry::createCube(CLOCK_WISE);
		mpShaderCommon = Shader::create("Resource/Shader/GLSL/Texture.vert", "Resource/Shader/GLSL/Texture.frag", POSITION_NORMAL_TEXTURE);
		mpTextureDiiffse = Texture2D::create("Resource/Texture/Pure.png");
		//* 
		glGenTextures(1, &mColorTex);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mColorTex);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, mSamples, GL_RGBA8, Utils::getWindowWidth(), Utils::getWindowHeight(), false);
		
		glGenTextures(1, &mDepthTex);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mDepthTex);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, mSamples, GL_DEPTH24_STENCIL8, Utils::getWindowWidth(), Utils::getWindowHeight(), false);
		//
		glGenFramebuffers(1, &mFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, mColorTex, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, mDepthTex, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//*/
		// Above is equivalent to :
		// mpRT = RenderTarget::createMultisample(Utils::getWindowWidth(), Utils::getWindowHeight(), 4, 1);
	}
	virtual void onLoop(shared_ptr<Camera> camera) {
		//
		glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
		Utils::clear();
		mpTextureDiiffse->use(0);
		mpCube->draw(mpShaderCommon);
		//
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, mFBO);
		glDrawBuffer(GL_BACK);
		glBlitFramebuffer(0, 0, Utils::getWindowWidth(), Utils::getWindowHeight(), 0, 0, Utils::getWindowWidth(), Utils::getWindowHeight(), GL_COLOR_BUFFER_BIT, mFilter);
	}
	virtual void onEnter() {
		printf("MSAA Enter\n");
		Utils::clearColor(1.0f, 0.0f, 0.0f);
	};
	virtual void onLeave() {
		printf("MSAA Leave\n");
	};
public:
	GLuint mSamples;
	GLenum mFilter;
protected:
	GLuint mColorTex, mDepthTex, mFBO;
	shared_ptr<Shape> mpCube;
	shared_ptr<Shader> mpShaderCommon;
	shared_ptr<Texture> mpTextureDiiffse;

};


#endif // DEMO_MSAA_HPP