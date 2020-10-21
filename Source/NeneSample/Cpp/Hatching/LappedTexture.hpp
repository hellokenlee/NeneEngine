/*Copyright reserved by KenLee@2020 ken4000kl@gmail.com*/
#ifndef LAPPED_TEXTURE
#define LAPPED_TEXTURE

#include <set>
#include <deque>
#include <time.h>
#include <stdlib.h>

#include "NeneEngine/Debug.h"
#include "NeneEngine/Nene.h"

#include "LappedTexturePatch.h"

namespace lappedtexture
{
	struct CustomCBDS
	{
		NNVec3 tangent;
		NNFloat _padding0;
		NNVec4 color;
	};

	bool g_shader_update = false;
	bool g_need_add_patch = false;
	bool g_need_grow_patch = false;
	int g_viewing_patch_index = 0;

	std::set<NNUInt> g_candidate_faces;

	std::shared_ptr<StaticMesh> g_bunny = nullptr;

	float g_tangent[3] = { 0.0f, 1.0f, 0.0f };

	std::vector<LappedTexturePatch> g_patches;
	std::vector<std::shared_ptr<Shader>> g_shaders;

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
		else if(k_event->mKey == NNKeyMap(EQUAL))
		{
			g_need_add_patch = true;
		}
		else if (k_event->mKey == NNKeyMap(MINUS))
		{
			g_need_grow_patch = true;
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
			ImGui::Text("Info:");
			ImGui::Text("%.2f ms/frame (%.0f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			//
			ImGui::Text("Tangent: ");
			ImGui::SliderFloat3("  ", g_tangent, -1.0f, 1.0f);
			//
			ImGui::Text("Patch View: ");
			if (g_patches.size() == 0)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}
			ImGui::SliderInt("    ", &g_viewing_patch_index, 0, g_patches.size() - 1);
			if (g_patches.size() == 0)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
			//
			ImGui::Text("Patch Control: ");
			if (ImGui::Button("Add Patch"))
			{
				g_need_add_patch = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("Grow Patch"))
			{
				g_need_grow_patch = true;
			}
		}
		ImGui::End();
	}

	void AddPatch()
	{
		LappedTexturePatch patch;
		patch.Initialize(g_bunny->GetMeshes()[0], g_candidate_faces);
		g_patches.emplace_back(patch);
	}

	void InitPatch()
	{
		//
		g_candidate_faces.clear();
		//
		std::shared_ptr<Mesh> mesh = g_bunny->GetMeshes()[0];
		//
		std::vector<NNUInt>& indices = mesh->GetIndexData();
		std::vector<Vertex>& vertices = mesh->GetVertexData();
		//
		for (NNUInt f = 0; f < NNUInt(indices.size()) / 3; ++f)
		{
			g_candidate_faces.insert(f);
		}
	}

	void GrowPatch(const NNUInt index)
	{
		g_patches[g_viewing_patch_index].Grow(g_candidate_faces);
	}
	
	void Main()
	{
		//
		srand((unsigned int)time(nullptr));
		//
		Utils::Init("LappedTexture", 1600, 900);
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
		g_bunny = StaticMesh::Create("Resource/Mesh/bunny/bunny.obj", 30.0f);
		//
		auto shader_debug = Shader::Create("Resource/Shader/GLSL/Debug.vert", "Resource/Shader/GLSL/Debug.frag", NNVertexFormat::POSITION_NORMAL_TEXTURE, true);
		shader_debug->AddOptionalShader("Resource/Shader/GLSL/Debug.geom", NNShaderType::GEOMETRY_SHADER, true);
		g_shaders.push_back(shader_debug);
		auto shader_flat = Shader::Create("Resource/Shader/GLSL/Flat.vert", "Resource/Shader/GLSL/Flat.frag", NNVertexFormat::POSITION_NORMAL_TEXTURE);
		g_shaders.push_back(shader_flat);
		auto shader_patch = Shader::Create("Resource/Shader/GLSL/Patch.vert", "Resource/Shader/GLSL/Patch.frag", NNVertexFormat::POSITION_NORMAL_TEXTURE);
		g_shaders.push_back(shader_patch);
		auto shader_2d_color = Shader::Create("Resource/Shader/GLSL/2DColor.vert", "Resource/Shader/GLSL/2DColor.frag", NNVertexFormat::POSITION_NORMAL_TEXTURE);
		g_shaders.push_back(shader_2d_color);
		auto shader_2d_texture = Shader::Create("Resource/Shader/GLSL/2DTexture.vert", "Resource/Shader/GLSL/2DTexture.frag", NNVertexFormat::POSITION_NORMAL_TEXTURE);
		g_shaders.push_back(shader_2d_texture);
		//
		auto tetxure2 = Texture2D::Create("Resource/Texture/checkerboard_s.png");
		//
		auto quad = Geometry::CreateQuad();
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
		InitPatch();
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
				CustomConstantBuffer.Update(NNConstantBufferSlot::CUSTOM_DATA_SLOT);
				LightConstantBuffer.Update(NNConstantBufferSlot::CUSTOM_LIGHT_SLOT);
				NeneCB::Instance().PerFrame().Update(NNConstantBufferSlot::PER_FRAME_SLOT);
			}
			
			//
			{
				//
				Utils::Clear();
				ca->Draw();
				// g_bunny->Draw(shader_flat);
				// Draw the tex
				tetxure2->Use(0);
				quad->Draw(shader_2d_texture);
				// Draw the patches
				tetxure2->Use(0);
				shader_patch->Use();
				{
					for (auto& patch : g_patches)
					{
						patch.DrawMesh(NNDrawMode::NN_TRIANGLE);
					}
				}
				// Draw viewing patch on texture for debug
				if (g_viewing_patch_index < g_patches.size())
				{
					tetxure2->Use(0);
					shader_2d_color->Use();
					{
						CustomConstantBuffer.Data().color = NNVec4(0.0, 1.0, 0.0, 1.0);
						CustomConstantBuffer.Update(NNConstantBufferSlot::CUSTOM_DATA_SLOT);
						g_patches[g_viewing_patch_index].DrawMesh(NN_LINE_LOOP);
						CustomConstantBuffer.Data().color = NNVec4(0.0, 0.0, 1.0, 1.0);
						CustomConstantBuffer.Update(NNConstantBufferSlot::CUSTOM_DATA_SLOT);
						g_patches[g_viewing_patch_index].DrawEdge(NN_LINE);
					}
				}
				//
				controls->Draw();
			}
			//
			Utils::SwapBuffers();
			//
			if (g_shader_update)
			{
				g_shader_update = false;
				printf("========== >>> Compiling Shaders >>> ===========\n");

				for (auto shader : g_shaders)
				{
					shader->Recompile();
				}

				printf("========== >>> Compiling Shaders <<< ===========\n");
			}
			if (g_need_add_patch)
			{
				g_need_add_patch = false;
				AddPatch();
			}
			if (g_need_grow_patch)
			{
				g_need_grow_patch = false;
				GrowPatch(g_viewing_patch_index);
			}
		}
		// 
		Utils::Terminate();
	}
}

#endif // LAPPED_TEXTURE
