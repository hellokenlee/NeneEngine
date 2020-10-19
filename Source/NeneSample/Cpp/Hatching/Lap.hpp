/*Copyright reserved by KenLee@2020 ken4000kl@gmail.com*/
#ifndef HATCHING_LAP_HPP
#define HATCHING_LAP_HPP

#include "NeneEngine/Debug.h"
#include "NeneEngine/Nene.h"

namespace lap
{
	struct CustomCBDS
	{
		NNVec3 tangent;
	};

	struct Patch
	{
		//
		std::vector<NNUInt> m_indices;
		std::vector<Vertex> m_vertices;
		//
		std::vector<std::tuple<NNUInt, NNUInt>> m_outer_edges;
	};
	
	bool g_shader_update = false;
	std::shared_ptr<StaticMesh> g_bunny = nullptr;

	float g_tangent[3] = { 0.0f, 1.0f, 0.0f };

	void KeyboardControl(std::shared_ptr<BaseEvent> eve)
	{
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
	void DrawGraphicUserInterfaces()
	{
		ImGui::Begin("Control", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		{
			//
			ImGui::SetWindowPos(ImVec2(10, 10));
			ImGui::SetWindowSize(ImVec2(320, 240));
			//
			ImGui::Text("Tangent: ");
			ImGui::SliderFloat3("  ", g_tangent, -1.0f, 1.0f);
		}
		ImGui::End();
	}

	void AddPatch()
	{
		// 随机选一个面

		//
	}

	void GrowPatch()
	{
		auto& meshes = g_bunny->GetMeshes();
		
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
		auto controls = UserInterface::Create(DrawGraphicUserInterfaces);
		//
		cc->m_speed = 3.0f;
		cc->SetYaw(4.0f);
		cc->SetPitch(-0.7f);
		cc->SetPosition(NNVec3(4.0f, 5.0f, 4.0f));
		//
		g_bunny = StaticMesh::Create("Resource/Mesh/bunny/bunny.obj");
		//
		auto shader0 = Shader::Create("Resource/Shader/GLSL/Debug.vert", "Resource/Shader/GLSL/Debug.frag", NNVertexFormat::POSITION_NORMAL_TEXTURE, true);
		shader0->AddOptionalShader("Resource/Shader/GLSL/Debug.geom", NNShaderType::GEOMETRY_SHADER, true);
		auto shader1 = Shader::Create("Resource/Shader/GLSL/Flat.vert", "Resource/Shader/GLSL/Flat.frag", NNVertexFormat::POSITION_NORMAL_TEXTURE);
		//
		g_bunny->ScaleTo(30.0);
		//
		ConstantBuffer<LightCBDS> LightConstantBuffer;
		LightConstantBuffer.Data().ltype = 123.456f;
		LightConstantBuffer.Data().range = 1000.0f;
		LightConstantBuffer.Data().color = NNVec4(1.0f, 1.0f, 1.0f, 1.0f);
		LightConstantBuffer.Data().attenuation = 1000.0f;
		LightConstantBuffer.Data().position = NNVec4(3.0f, 4.0f, 4.0f, 0.0f);
		//
		ConstantBuffer<CustomCBDS> CustomConstantBuffer;
		//
		while (!Utils::WindowShouldClose()) 
		{
			//
			Utils::Update();
			//
			cc->Update();
			cc->GetCamera()->Use();
			{
				NNVec3 tangent(g_tangent[0], g_tangent[1], g_tangent[2]);
				CustomConstantBuffer.Data().tangent = NNNormalize(tangent);
				LightConstantBuffer.Update(NNConstantBufferSlot::CUSTOM_LIGHT_SLOT);
				CustomConstantBuffer.Update(NNConstantBufferSlot::CUSTOM_FRAME_SLOT);
				NeneCB::Instance().PerFrame().Update(NNConstantBufferSlot::PER_FRAME_SLOT);
			}
			
			//
			{
				Utils::Clear();
				ca->Draw();
				g_bunny->Draw(shader1);
				g_bunny->Draw(shader0);
				controls->Draw();
			}
			//
			Utils::SwapBuffers();
			//
			if (g_shader_update)
			{
				g_shader_update = false;
				printf("========== >>> Compiling Shaders >>> ===========\n");

				shader0->Recompile();
				shader1->Recompile();

				printf("========== >>> Compiling Shaders <<< ===========\n");
			}
		}
		// 
		Utils::Terminate();
	}
}

#endif // HATCHING_LAP_HPP
