/*Copyright reserved by KenLee@2018 ken4000kl@gmail.com*/
#include "../../NeneEngine/Debug.h"
#include "../../NeneEngine/Nene.h"

using namespace std;

void myCallback(shared_ptr<BaseEvent> eve) {
	shared_ptr<KeyboardEvent> k_event = dynamic_pointer_cast<KeyboardEvent>(eve);
	if (k_event->mKey == NNKeyMap(ESCAPE)) {
		Utils::SetWindowShouldClose(true);
	}
}


int main() {
	// 初始化
	Utils::Init("NeneSample", 800, 600);
	// 背景色
	Utils::ClearColor(0.1f, 0.1f, 0.1f);
	//
	auto ui = GUI::Create();
	// 增加回调
	Keyboard::Instance().OnPress().AddCallbackFunc(myCallback);
	//
	NeneCB::Instance();
	//
	auto cc = CameraController::Create();
	auto ca = CoordinateAxes::Create(100.0f, 10.0f);
	//
	auto cube = Geometry::CreateCube();
	auto quad = Geometry::CreateQuad();
	
	auto shader_shadow = Shader::Create("../../Resource/Shader/GLSL/Shadow.vert", "../../Resource/Shader/GLSL/Shadow.frag", POSITION_NORMAL_TEXTURE, true);
	auto shader_forward = Shader::Create("../../Resource/Shader/GLSL/ShadowedCommon.vert", "../../Resource/Shader/GLSL/ShadowedCommon.frag", POSITION_NORMAL_TEXTURE, true);
	auto tx = Texture2D::Create("../../Resource/Texture/snow.jpg");
	//
	auto light = Light::Create();
	light->mDirection = NNVec3(10.0f, 10.0f, 10.0f);
	auto sm = ShadowMap::Create(3.0f, 512, 512);
	sm->SetLight(light);
	quad->RotateX(M_PI_OVER_2);
	quad->ScaleTo(50.0f);
	cube->ScaleTo(0.5f);
	quad->MoveTo(NNVec3(0.0f, -0.1f, 0.0f));
	cube->MoveTo(NNVec3(0.0f, 0.4f, 0.0f));
	cc->SetSpeed(2.0f);
	// 主循环
	while (!Utils::WindowShouldClose()) {
		// 处理 IO
		Utils::Update();
		// Camera
		cc->Update();
		cc->GetCamera()->Use();
		NeneCB::Instance().PerFrame().Update(PER_FRAME_SLOT);
		// Shadow Pass
		glViewport(0, 0, 512, 512);
		sm->Begin();
		{
			Utils::Clear();
			cube->Draw(shader_shadow);
			quad->Draw(shader_shadow);
		}
		sm->End();
		glViewport(0, 0, 800, 600);
		// Forward Pass
		{
			Utils::Clear();
			ca->Draw();
			tx->Use(0);
			sm->Use(1);
			cube->Draw(shader_forward);
			quad->Draw(shader_forward);
		}
		//
		ui->Draw();
		// 交换缓冲
		Utils::SwapBuffers();
	}
	// 释放资源
	Utils::Terminate();
	//
	return 0;
}



