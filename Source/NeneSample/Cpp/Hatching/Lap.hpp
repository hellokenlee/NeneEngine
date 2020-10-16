/*Copyright reserved by KenLee@2020 ken4000kl@gmail.com*/
#ifndef HATCHING_LAP_HPP
#define HATCHING_LAP_HPP

#include "NeneEngine/Debug.h"
#include "NeneEngine/Nene.h"

namespace lap
{
	bool g_shader_update = false;
	
	const char* g_vertexshader_path = "Resource/Shader/GLSL/Debug.vert";
	const char* g_geometryshader_path = "Resource/Shader/GLSL/Debug.geom";
	const char* g_fragmentshader_path = "Resource/Shader/GLSL/Debug.frag";

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
		Utils::Init("LappedTextures", 1600, 900);
		Utils::ClearColor(0.1f, 0.1f, 0.1f);
		Keyboard::Instance().OnPress().AddCallbackFunc(KeyboardControl);
		//
		auto cc = CameraController::Create();
		auto ca = CoordinateAxes::Create(100.0f, 10.0f);
		//
		cc->m_speed = 3.0f;
		cc->SetYaw(4.0f);
		cc->SetPitch(-0.7f);
		cc->SetPosition(NNVec3(4.0f, 5.0f, 4.0f));
		//
		auto bunny = Model::Create("Resource/Mesh/bunny/bunny.obj");
		//
		auto shader0 = Shader::Create(g_vertexshader_path, g_fragmentshader_path, NNVertexFormat::POSITION_NORMAL_TEXTURE, true);
		shader0->AddOptionalShader(g_geometryshader_path, NNShaderType::GEOMETRY_SHADER, true);
		auto shader1 = Shader::Create("Resource/Shader/GLSL/Flat.vert", "Resource/Shader/GLSL/Flat.frag", NNVertexFormat::POSITION_NORMAL_TEXTURE);
		//
		bunny->ScaleTo(30.0);
		//
		ConstantBuffer<LightCBDS> LightConstantBuffer;
		LightConstantBuffer.Data().ltype = 123.456f;
		LightConstantBuffer.Data().range = 1000.0f;
		LightConstantBuffer.Data().color = NNVec4(1.0f, 1.0f, 1.0f, 1.0f);
		LightConstantBuffer.Data().attenuation = 1000.0f;
		LightConstantBuffer.Data().position = NNVec4(3.0f, 4.0f, -4.0f, 0.0f);
		//
		while (!Utils::WindowShouldClose()) {
			//
			Utils::Update();
			//
			cc->Update();
			cc->GetCamera()->Use();
			LightConstantBuffer.Update(NNConstantBufferSlot::CUSTOM_LIGHT_SLOT);
			NeneCB::Instance().PerFrame().Update(NNConstantBufferSlot::PER_FRAME_SLOT);
			//
			{
				Utils::Clear();
				ca->Draw();
				bunny->Draw(shader1);
				bunny->Draw(shader0);
			}
			//
			Utils::SwapBuffers();
			//
			if (g_shader_update)
			{
				printf("========== Compiling Shaders >>> ===========\n");
				auto new_shader = Shader::Create(
					g_vertexshader_path,
					g_fragmentshader_path,
					NNVertexFormat::POSITION_NORMAL_TEXTURE,
					false
				);
				new_shader->AddOptionalShader(g_geometryshader_path, NNShaderType::GEOMETRY_SHADER, true);
				if (new_shader != nullptr)
				{
					shader0 = new_shader;
				}
				g_shader_update = false;
				printf("========== Compiling Shaders <<< ===========\n");
			}
		}
		// 
		Utils::Terminate();
	}
}

#endif // HATCHING_LAP_HPP
