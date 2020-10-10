/*Copyright reserved by KenLee@2020 ken4000kl@gmail.com*/
#ifndef HATCHING_MAIN_HPP
#define HATCHING_MAIN_HPP

#include "NeneEngine/Debug.h"
#include "NeneEngine/Nene.h"

namespace hatching
{
	bool g_shader_update = false;
	
	void KeyboardControl(std::shared_ptr<BaseEvent> eve) {
		std::shared_ptr<KeyboardEvent> k_event = std::dynamic_pointer_cast<KeyboardEvent>(eve);
		if (k_event->mKey == NNKeyMap(ESCAPE))
		{
			Utils::SetWindowShouldClose(true);
		}
		else if (k_event->mKey == NNKeyMap(F5))
		{
			g_shader_update = true;
		}
	}


	void Main()
	{
		//
		Utils::Init("Hatching", 800, 600);
		Utils::ClearColor(0.1f, 0.1f, 0.1f);
		Keyboard::Instance().OnPress().AddCallbackFunc(KeyboardControl);
		//
		NeneCB::Instance();
		//
		auto cc = CameraController::Create();
		auto ca = CoordinateAxes::Create(100.0f, 10.0f);
		//
		auto cube = Geometry::CreateCube();
		auto shader = Shader::Create(
			"../../Resource/Shader/GLSL/Common.vert", 
			"../../Resource/Shader/GLSL/Common.frag", 
			NNVertexFormat::POSITION_NORMAL_TEXTURE
		);
		auto texture = Texture2D::Create("../../Resource/Texture/hatching01.png");
		//
		cc->m_speed = 1.5f;
		// 
		while (!Utils::WindowShouldClose()) {
			//
			Utils::Update();
			//
			cc->Update();
			cc->GetCamera()->Use();
			NeneCB::Instance().PerFrame().Update(NNConstantBufferSlot::PER_FRAME_SLOT);
			//
			{
				Utils::Clear();
				ca->Draw();
				//texture->Use(0);
				cube->Draw(shader);
			}
			//
			Utils::SwapBuffers();
			//
			if (g_shader_update)
			{
				auto new_shader = Shader::Create(
					"../../Resource/Shader/GLSL/Common.vert",
					"../../Resource/Shader/GLSL/Common.frag",
					NNVertexFormat::POSITION_NORMAL_TEXTURE
				);
				if (new_shader != nullptr)
				{
					shader = new_shader;
				}
				g_shader_update = false;
			}
		}
		// 
		Utils::Terminate();
	}
}

#endif // HATCHING_MAIN_HPP