/*Copyright reserved by KenLee@2020 ken4000kl@gmail.com*/
#ifndef LAPPED_TEXTURE
#define LAPPED_TEXTURE

#include <set>
#include <deque>
#include <time.h>
#include <stdlib.h>

#include "NeneEngine/Debug.h"
#include "NeneEngine/Nene.h"

#include "LappedTextureMesh.h"


namespace lappedtexture
{
	struct CustomCBDS
	{
		NNVec3 tangent;
		NNFloat _padding0;
		NNVec4 color;
	};

	bool g_consecutive_add = false;
	bool g_consecutive_grow = false;
	bool g_need_add_patch = false;
	bool g_need_grow_patch = false;
	bool g_need_update_coverage = false;

	NNVec3 g_camera_pos;
	NNVec2 g_camera_rot;

	float g_tangent[3] = { 0.0f, 1.0f, 0.0f };
	
	int g_viewing_patch_index = 0;
	std::shared_ptr<LappedTextureMesh> g_lapped_mesh = nullptr;


	void KeyboardControl(std::shared_ptr<BaseEvent> eve)
	{
		std::shared_ptr<KeyboardEvent> k_event = std::dynamic_pointer_cast<KeyboardEvent>(eve);
		if (k_event->mKey == NNKeyMap(ESCAPE))
		{
			Utils::SetWindowShouldClose(true);
		}
		else if (k_event->mKey == NNKeyMap(F5))
		{
			Shader::RecompileAllShaders();
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
			ImGui::SetWindowSize(ImVec2(320, 400));
			//
			ImGui::Text("Info:");
			ImGui::Text("%.2f ms/frame (%.0f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			//
			ImGui::Text("Camera: ");
			ImGui::Text("(%.1f, %.1f, %.1f) | (%.1f, %.1f) ", g_camera_pos.x, g_camera_pos.y, g_camera_pos.z, g_camera_rot.x, g_camera_rot.y);
			//
			ImGui::Text("Tangent: ");
			ImGui::SliderFloat3("  ", g_tangent, -1.0f, 1.0f);
			//
			if (g_lapped_mesh)
			{
				if (g_lapped_mesh->PatchCount() > 0)
				{
					ImGui::Text("Patch View: ");
					ImGui::SliderInt("    ", &g_viewing_patch_index, 0, int(g_lapped_mesh->PatchCount()) - 1);
				}
				//
				ImGui::Text("Patch Control: ");
				ImGui::Checkbox("Consecutive Add", &g_consecutive_add);
				ImGui::Checkbox("Consecutive Grow", &g_consecutive_grow);
				//
				if (ImGui::Button("Add Patch"))
				{
					g_need_add_patch = true;
				}
				ImGui::SameLine();
				if (ImGui::Button("Grow Patch"))
				{
					g_need_grow_patch = true;
				}
				/*
				ImGui::SameLine();
				if (ImGui::Button("Update Coverage"))
				{
					g_need_update_coverage = true;
				}
				*/
			}
		}
		ImGui::End();
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
		cc->SetYaw(0.3f);
		cc->SetPitch(-0.3f);
		cc->SetPosition(NNVec3(-4.7f, 4.7f, -0.2f));
		//
		auto bunny = StaticMesh::Create("Resource/Mesh/bunny/bunny_with_uv.obj", 30.0f);
		//
		auto shader_debug = Shader::Create("Resource/Shader/GLSL/Debug.vert", "Resource/Shader/GLSL/Debug.frag", NNVertexFormat::POSITION_NORMAL_TEXTURE, true);
		shader_debug->AddOptionalShader("Resource/Shader/GLSL/Debug.geom", NNShaderType::GEOMETRY_SHADER, true);
		//
		auto shader_flat = Shader::Create("Resource/Shader/GLSL/Flat.vert", "Resource/Shader/GLSL/Flat.frag", NNVertexFormat::POSITION_NORMAL_TEXTURE);
		//
		auto shader_3d_color = Shader::Create("Resource/Shader/GLSL/3DColor.vert", "Resource/Shader/GLSL/3DColor.frag", NNVertexFormat::POSITION_NORMAL_TEXTURE);
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
		// g_lapped_mesh = std::make_shared<LappedTextureMesh>(bunny->GetMeshes()[0]);
		g_lapped_mesh = std::make_shared<LappedTextureMesh>("Resource/Mesh/bunny/bunny_with_uv.obj");
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
			
			// Main Pass
			{
				//
				Utils::Clear();
				ca->Draw();
				//
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					CustomConstantBuffer.Data().color = NNVec4(0.0, 1.0, 1.0, 1.0);
					CustomConstantBuffer.Update(NNConstantBufferSlot::CUSTOM_DATA_SLOT);
					bunny->Draw(shader_3d_color);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}
				//
				
				// Draw the patches
				{
					CustomConstantBuffer.Data().color = NNVec4(0.0, 1.0, 0.0, 1.0);
					CustomConstantBuffer.Update(NNConstantBufferSlot::CUSTOM_DATA_SLOT);
					g_lapped_mesh->Draw();
					g_lapped_mesh->DrawDebug(g_viewing_patch_index);
				}
				
				// 
				/*
				if (g_need_update_coverage)
				{
					g_need_update_coverage = false;
					g_lapped_mesh->DrawAndCalcFaceCoverage();
				}
				*/
				//
				g_lapped_mesh->DrawAndCalcFaceCoverage();
			}

			// Interface Pass
			{
				g_camera_pos = cc->GetCamera()->GetPosition();
				g_camera_rot.x = cc->GetCamera()->GetYaw();
				g_camera_rot.y = cc->GetCamera()->GetPitch();
				controls->Draw();
			}
			//
			Utils::SwapBuffers();
			
			// Handle Interfaces
			if (g_need_add_patch && !g_need_grow_patch)
			{
				g_need_add_patch = false;
				//
				g_viewing_patch_index = g_lapped_mesh->AddPatch();
				//
				if (g_consecutive_add)
				{
					g_need_grow_patch = true;
				}
			}
			if (g_need_grow_patch)
			{
				if (NNUInt(g_viewing_patch_index) < g_lapped_mesh->PatchCount())
				{
					//
					g_lapped_mesh->GetPatch(g_viewing_patch_index).Grow();
					//
					if (g_consecutive_grow)
					{
						if (g_lapped_mesh->GetPatch(g_viewing_patch_index).IsGrown())
						{
							g_need_grow_patch = false;
							if (g_consecutive_add)
							{
								g_need_add_patch = true;
							}
							g_lapped_mesh->SetNeedToUpdateFaceCoverage();
						}
					}
					else
					{
						g_need_grow_patch = false;
					}
				}
				else
				{
					g_need_grow_patch = false;
				}
			}
		}
		
		// 
		Utils::Terminate();
	}
}

#endif // LAPPED_TEXTURE
