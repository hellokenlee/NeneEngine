/*Copyright reserved by KenLee@2018 ken4000kl@gmail.com*/
#ifndef UNIT_SPHERE_MAIN_HPP
#define UNIT_SPHERE_MAIN_HPP

#include "NeneEngine/Debug.h"
#include "NeneEngine/Nene.h"

namespace Simple
{
	void MyCallback(std::shared_ptr<BaseEvent> eve) 
	{
		std::shared_ptr<KeyboardEvent> k_event = std::dynamic_pointer_cast<KeyboardEvent>(eve);
		if (k_event->mKey == NNKeyMap(ESCAPE)) {
			Utils::SetWindowShouldClose(true);
		}
	}

	int Main() 
	{
		// 初始化
		Utils::Init("UnitSphere", 800, 600);
		// 背景色
		Utils::ClearColor(0.1f, 0.1f, 0.1f);
		// 增加回调
		Keyboard::Instance().OnPress().AddCallbackFunc(MyCallback);
		//
		NeneCB::Instance();
		//
		auto cc = CameraController::Create();
		auto ca = CoordinateAxes::Create(100.0f, 10.0f);
		//
		auto cube = Geometry::CreateCube();
		auto quad = Geometry::CreateQuad();
		auto shader = Shader::Create("Resource/Shader/GLSL/Common.vert", "Resource/Shader/GLSL/Common.frag", POSITION_NORMAL_TEXTURE, true);
		auto shadowed_shader = Shader::Create("Resource/Shader/GLSL/ShadowedCommon.vert", "Resource/Shader/GLSL/ShadowedCommon.frag", POSITION_NORMAL_TEXTURE, true);
		auto tx = Texture2D::Create("Resource/Texture/hatching01.png");
		//
		auto sm = ShadowMap::Create(10.0f, 512, 512);
		// 主循环
		while (!Utils::WindowShouldClose()) {
			// 处理 IO
			Utils::Update();
			//
			cc->Update();
			cc->GetCamera()->Use();
			NeneCB::Instance().PerFrame().Update(NNConstantBufferSlot::PER_FRAME_SLOT);
			//
			{
				Utils::Clear();
				tx->Use(0);
				//sm->Use(1);
				cube->Draw(shader);
				quad->Draw(shader);
			}
			//
			// 交换缓冲
			Utils::SwapBuffers();
		}
		// 释放资源
		Utils::Terminate();
		//
		return 0;
	}
};

#endif //  UNIT_SPHERE_MAIN_HPP
