/*Copyright reserved by KenLee@2018 ken4000kl@gmail.com*/
#ifndef SIMPLE_MAIN_HPP
#define SIMPLE_MAIN_HPP

#include "NeneEngine/Debug.h"
#include "NeneEngine/Nene.h"

namespace UnitSphere
{
	float g_camera_position[3] = { 0.0f, 0.0f, 0.0f };
	float g_camera_rotation[2] = { 0.0f, 0.0f };

	constexpr int ModeNum = 2;

	int g_current_mode = 0;

	int g_current_bit_count = 8;
	
	std::shared_ptr<Shape> g_shapes[ModeNum]
	{
		nullptr,
		nullptr,
	};

	const char* g_mode_string[ModeNum] = {
		"XYZ",
		"LatLong",
	};



	void MyCallback(std::shared_ptr<BaseEvent> eve)
	{
		std::shared_ptr<KeyboardEvent> k_event = std::dynamic_pointer_cast<KeyboardEvent>(eve);
		if (k_event->mKey == NNKeyMap(ESCAPE)) 
		{
			Utils::SetWindowShouldClose(true);
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
			ImGui::Text("Mode: ");
			ImGui::Combo("", &g_current_mode, g_mode_string, ModeNum);
			//
			ImGui::Text("Bits Count: ");
			ImGui::SliderInt("", &g_current_bit_count, 1, 8);
	
		}
		ImGui::End();
	}

	std::shared_ptr<Shape> CreatePointCloudLatLong(const int BitCount)
	{
		std::vector<float> Vertices;
		for (int U = 0; U < BitCount; ++U)
		{
			float UU = (static_cast<float>(U) / static_cast<float>(BitCount));
			float Pitch = UU * M_PI;
			NNMat4 RotationAlongAxisX = NNCreateRotationX(NNMat4Identity, Pitch);
			for (int V = 0; V < BitCount; ++V)
			{
				float VV = (static_cast<float>(V) / static_cast<float>(BitCount));
				float Yaw = VV * M_PI_TIMES_2;
				NNMat4 RotationAlongAxisZ = NNCreateRotationZ(NNMat4Identity, Yaw);
				// Unit Vector Z+
				NNVec4 Position = NNVec4(0.0f, 0.0f, 1.0f, 1.0f);
				Position = NNMat4MulVec4(RotationAlongAxisX, Position);
				Position = NNMat4MulVec4(RotationAlongAxisZ, Position);
				//
				NNVec3 Pos = NNVec3(Position);
				Vertices.push_back(Pos.x);
				Vertices.push_back(Pos.y);
				Vertices.push_back(Pos.z);

				// Col
				Vertices.push_back(UU);
				Vertices.push_back(VV);
				Vertices.push_back(0.5);
			}
		}
		auto res = Shape::Create(Vertices, POSITION_NORMAL);
		res->SetDrawMode(NNDrawMode::NN_POINT);
		return res;
	}

	std::shared_ptr<Shape> CreatePointCloudXYSignZ(const int BitCount)
	{

		std::vector<float> Vertices;
		for (int X = 0; X < BitCount; ++X)
		{
			for (int Y = 0; Y < BitCount; ++Y)
			{
				float XX = static_cast<float>(X) * 2.0f / BitCount - 1.0f;
				float YY = static_cast<float>(Y) * 2.0f / BitCount - 1.0f;
				float ZZP = sqrt(1.0f - XX * XX - YY * YY);
				float ZZN = -sqrt(1.0f - XX * XX - YY * YY);
				// Pos
				Vertices.push_back(XX);
				Vertices.push_back(YY);
				Vertices.push_back(ZZP);

				// Col
				Vertices.push_back(XX + 1.0f * 0.5f);
				Vertices.push_back(YY + 1.0f * 0.5f);
				Vertices.push_back(0.5);

				// Pos
				Vertices.push_back(XX);
				Vertices.push_back(YY);
				Vertices.push_back(ZZN);

				// Col
				Vertices.push_back(XX + 1.0f * 0.5f);
				Vertices.push_back(YY + 1.0f * 0.5f);
				Vertices.push_back(1.0);
			}
		}
		auto res = Shape::Create(Vertices, POSITION_NORMAL);
		res->SetDrawMode(NNDrawMode::NN_POINT);
		return res;
	}

	int Main()
	{
		// 初始化
		Utils::Init("UnitSphere", 1600, 900);
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
		auto shader = Shader::Create(
			"Resource/Shader/GLSL/PointCloud.vert", 
			"Resource/Shader/GLSL/PointCloud.frag", 
			POSITION_NORMAL_TEXTURE, true
		);
		//
		int bit_count = g_current_bit_count;
		g_shapes[0] = CreatePointCloudXYSignZ(1 << bit_count);
		g_shapes[1] = CreatePointCloudLatLong(1 << bit_count);
		auto controls = UserInterface::Create(DrawGraphicUserInterfaces);

		//
		cc->SetPosition(NNVec3(2.0f, 1.5f, 2.0f));
		cc->SetYaw(3.9f);
		cc->SetPitch(-0.5f);
		cc->SetLocked(true);
		Mouse::Instance().SetCursorShow(true);

		
		// 主循环
		while (!Utils::WindowShouldClose()) 
		{
			// 处理 IO
			Utils::Update();
			//
			cc->Update();
			cc->GetCamera()->Use();
			NeneCB::Instance().PerFrame().Update(NNConstantBufferSlot::PER_FRAME_SLOT);
			//
			{
				Utils::Clear();
				g_shapes[g_current_mode]->Draw(shader);
				ca->Draw();
			}

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
			{
				if (bit_count != g_current_bit_count)
				{
					bit_count = g_current_bit_count;
					g_shapes[0] = CreatePointCloudXYSignZ(1 << bit_count);
					g_shapes[1] = CreatePointCloudLatLong(1 << bit_count);
				}
			}
			// 交换缓冲
			Utils::SwapBuffers();
		}
		// 释放资源
		Utils::Terminate();
		//
		return 0;
	}
};

#endif // SIMPLE_MAIN_HPP
