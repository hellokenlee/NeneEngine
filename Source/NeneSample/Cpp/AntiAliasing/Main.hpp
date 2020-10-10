/*Copyright reserved by KenLee@2019 ken4000kl@gmail.com*/
#ifndef ANTIALIASING_MAIN_HPP
#define ANTIALIASING_MAIN_HPP

#include "NeneEngine/Debug.h"
#include "NeneEngine/Nene.h"

#include "DemoMSAA.hpp"
#include "DemoTSAA.hpp"
#include "DemoFXAA.hpp"
#include "DemoNOAA.hpp"

namespace antialiasing
{
	size_t gCurrIdx = 2;
	vector<shared_ptr<DemoBase>> gpDemos;
	
	void DrawGUI() {
		// 
		ImGui::Begin("Status", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::SetWindowPos(ImVec2(10, 10));
		ImGui::SetWindowSize(ImVec2(240, 180));
		// 
		ImGui::TextColored(ImVec4(0.411f, 1.0f, 0.956f, 1.0), "Info:");
		ImGui::Text("%.2f ms/frame (%.0f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//
		ImGui::TextColored(ImVec4(0.411f, 1.0f, 0.956f, 1.0), "Control:");
		static size_t nextIdx;
		if (ImGui::BeginCombo("", gpDemos[gCurrIdx]->mName.c_str()))
		{
			for (size_t i = 0; i < gpDemos.size(); ++i)
			{
				static bool is_selected;
				is_selected = (gpDemos[gCurrIdx]->mName.c_str() == gpDemos[i]->mName.c_str());
				if (ImGui::Selectable(gpDemos[i]->mName.c_str(), is_selected)) {
					nextIdx = i;
					if (nextIdx != gCurrIdx) {
						gpDemos[gCurrIdx]->onLeave();
						gpDemos[nextIdx]->onEnter();
					}
					gCurrIdx = nextIdx;
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	}

	void Main() {
		// 初始化
		Utils::Init("AA", 800, 600);
		//
		auto cc = CameraController::Create();
		//
		gpDemos.emplace_back(shared_ptr<DemoBase>(new DemoNOAA()));
		gpDemos.emplace_back(shared_ptr<DemoBase>(new DemoMSAA()));
		gpDemos.emplace_back(shared_ptr<DemoBase>(new DemoTSAA()));
		gpDemos.emplace_back(shared_ptr<DemoBase>(new DemoFXAA()));
		//
		for (auto p : gpDemos) {
			p->onInit();
		}
		//
		GUI panel;
		panel.mDrawFunc = DrawGUI;
		//
		gpDemos[gCurrIdx]->onEnter();
		// 主循环
		while (!Utils::WindowShouldClose()) {
			//
			Utils::PollEvents();
			Utils::Update();
			//
			cc->Update();
			cc->GetCamera()->Use();
			NeneCB::Instance().PerFrame.Update(NNConstantBufferSlot::PER_FRAME_SLOT);
			//
			gpDemos[gCurrIdx]->onLoop(cc->GetCamera());
			//
			panel.Draw();
			// 
			Utils::SwapBuffers();
		}
		//
		gCurrIdx = 0;
		while (!gpDemos.empty()) {
			gpDemos.pop_back();
		}
		// 释放资源
		Utils::Terminate();
	}
}

#endif // ANTIALIASING_MAIN_HPP
