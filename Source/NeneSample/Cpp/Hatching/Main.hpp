/*Copyright reserved by KenLee@2020 ken4000kl@gmail.com*/
#ifndef HATCHING_MAIN_HPP
#define HATCHING_MAIN_HPP

#include "NeneEngine/Debug.h"
#include "NeneEngine/Nene.h"


namespace hatching
{
	bool g_shader_update = false;
	float g_texcoord_scale = 5.0f;
	float g_light_position[3] = { 3.0f, 4.0f, -4.0f };
	float g_camera_position[3] = { 0.0f, 0.0f, 0.0f };
	float g_camera_rotation[2] = { 0.0f, 0.0f };
	float g_light_intensity = 1.0f;

	/* For <Real-Time Stroke Textures> Freud. et al.
	const char* g_vertexshader_path = "Resource/Shader/GLSL/FreudHatch.vert";
	const char* g_fragmentshader_path = "Resource/Shader/GLSL/FreudHatch.frag";
	//*/

	/* For <Real-Time Hatching> Praun et al.
	const char* g_vertexshader_path = "Resource/Shader/GLSL/PraunHatchOrigin.vert";
	const char* g_fragmentshader_path = "Resource/Shader/GLSL/PraunHatchOrigin.frag";
	//*/
	
	//* For <Fine Tone Control in Harward Hatching> Praun et al. Scheme1
	const char* g_vertexshader_path = "Resource/Shader/GLSL/PraunHatchScheme1.vert";
	const char* g_fragmentshader_path = "Resource/Shader/GLSL/PraunHatchScheme1.frag";
	//*/

	//const char* g_vertexshader_path = "Resource/Shader/GLSL/TextureVisualize.vert";
	//const char* g_fragmentshader_path = "Resource/Shader/GLSL/TextureVisualize.frag";

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

	void DrawGraphicUserInterfaces()
	{
		ImGui::Begin("Control", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		{
			//
			ImGui::SetWindowPos(ImVec2(10, 10));
			ImGui::SetWindowSize(ImVec2(320, 240));
			//
			ImGui::Text("Camera: ");
			ImGui::Text("(%.1f, %.1f, %.1f) | (%.1f, %.1f) ", g_camera_position[0], g_camera_position[1], g_camera_position[2], g_camera_rotation[0], g_camera_rotation[1]);
			//
			ImGui::Text("TexCoord: ");
			ImGui::SliderFloat(" ", &g_texcoord_scale, 1.0f, 10.0f);
			//
			ImGui::Text("LightPos: ");
			ImGui::SliderFloat3("  ", g_light_position, -10.0f, 10.0f);
			//
			ImGui::Text("LightIntensity: ");
			ImGui::SliderFloat("   ", &g_light_intensity, 0.0f, 2.0f);
		}
		ImGui::End();
	}

	void Main()
	{
		//
		Utils::Init("Hatching", 1600, 900);
		Utils::ClearColor(0.1f, 0.1f, 0.1f);
		Keyboard::Instance().OnPress().AddCallbackFunc(KeyboardControl);
		//
		NeneCB::Instance();
		//
		auto cc = CameraController::Create();
		auto ca = CoordinateAxes::Create(100.0f, 10.0f);
		auto controls = UserInterface::Create(DrawGraphicUserInterfaces);
		//
		auto quad = Geometry::CreateQuad();
		auto cube = Geometry::CreateCube();
		auto ball = Geometry::CreateSphereUV(30, 30);
		auto shader = Shader::Create(
			g_vertexshader_path,
			g_fragmentshader_path,
			NNVertexFormat::POSITION_NORMAL_TEXTURE
		);
		//
		static const size_t MIPMAP_LEVELS = 4;
		static const NNUInt MAX_TONE_LEVELS = 64;
		//
		NNChar filepath[256];
		std::vector<std::vector<std::string>> images(MIPMAP_LEVELS);
		//
		for (NNUInt mip = 0; mip < MIPMAP_LEVELS; ++mip)
		{
			NNUInt curr_tone_levels = MAX_TONE_LEVELS >> mip;
			for (NNUInt tone = 0; tone < curr_tone_levels; ++tone)
			{
				sprintf_s(filepath, "Resource/Texture/VTAM/MipMapLv%d/Tone%03d.bmp", mip, tone);
				images[mip].push_back(filepath);
			}
		}
		//
		auto tex_hatch_vol = Texture3D::Create(images);
		auto tex_hatch = Texture2D::Create({
			"Resource/Texture/TAM/default23.bmp"
		});
		auto tex_hatch_tone012 = Texture2D::Create({
			"Resource/Texture/BTAM/Default_Mip0_Tone012.png",
			"Resource/Texture/BTAM/Default_Mip1_Tone012.png", 
			"Resource/Texture/BTAM/Default_Mip2_Tone012.png", 
			"Resource/Texture/BTAM/Default_Mip3_Tone012.png"
		});
		auto tex_hatch_tone345 = Texture2D::Create({
			"Resource/Texture/BTAM/Default_Mip0_Tone345.png",
			"Resource/Texture/BTAM/Default_Mip1_Tone345.png",
			"Resource/Texture/BTAM/Default_Mip2_Tone345.png",
			"Resource/Texture/BTAM/Default_Mip3_Tone345.png"
		});
		//
		cc->SetPosition(NNVec3(4.0f, 5.0f, 4.0f));
		cc->SetYaw(4.0f);
		cc->SetPitch(-0.7f);
		cc->m_speed = 3.0f;
		ball->MoveTo(NNVec3(0.0, 1.0, 0.0));
		//
		ConstantBuffer<LightCBDS> LightConstantBuffer;
		LightConstantBuffer.Data().ltype = 123.456f;
		LightConstantBuffer.Data().range = 1000.0f;
		LightConstantBuffer.Data().color = NNVec4(1.0f, 1.0f, 1.0f, 1.0f);
		LightConstantBuffer.Data().attenuation = 1000.0f;
		// 
		while (!Utils::WindowShouldClose()) {
			//
			Utils::Update();
			//
			cc->Update();
			cc->GetCamera()->Use();
			//
			NeneCB::Instance().PerFrame().Data().texcoord_scale = g_texcoord_scale;
			NeneCB::Instance().PerFrame().Update(NNConstantBufferSlot::PER_FRAME_SLOT);
			//
			LightConstantBuffer.Data().color.r = g_light_intensity;
			LightConstantBuffer.Data().position.x = g_light_position[0];
			LightConstantBuffer.Data().position.y = g_light_position[1];
			LightConstantBuffer.Data().position.z = g_light_position[2];
			LightConstantBuffer.Update(NNConstantBufferSlot::CUSTOM_LIGHT_SLOT);

			/* 2001 Freu. et al.
			{

				Utils::Clear();
				ca->Draw();
				tex_hatch->Use(0);
				ball->Draw(shader);
			}
			//*/

			/* 2001 Praun et al.
			{
				Utils::Clear();
				ca->Draw();
				tex_hatch_tone012->Use(0);
				tex_hatch_tone345->Use(1);
				ball->Draw(shader);
			}
			//*/
			
			//* 2002 Praun et al. Scheme 1
			{
				Utils::Clear();
				ca->Draw();
				tex_hatch_vol->Use(0);
				ball->Draw(shader);
			}
			//*/

			//
			{
				g_camera_position[0] = cc->GetCamera()->GetPosition().x;
				g_camera_position[1] = cc->GetCamera()->GetPosition().y;
				g_camera_position[2] = cc->GetCamera()->GetPosition().z;
				g_camera_rotation[0] = cc->GetCamera()->GetYaw();
				g_camera_rotation[1] = cc->GetCamera()->GetPitch();
				controls->Draw();
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
					NNVertexFormat::POSITION_NORMAL_TEXTURE
				); 
				if (new_shader != nullptr)
				{
					shader = new_shader;
				}
				g_shader_update = false;
				printf("========== Compiling Shaders <<< ===========\n");
			}
		}
		// 
		Utils::Terminate();
	}
}

#endif // HATCHING_MAIN_HPP
