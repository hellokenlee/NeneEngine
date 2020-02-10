/*Copyright reserved by KenLee@2019 ken4000kl@gmail.com*/
#include "../../Source/Debug.h"
#include "../../Source/Nene.h"

#include "DemoMSAA.hpp"
#include "DemoTSAA.hpp"
#include "DemoFXAA.hpp"
#include "DemoNOAA.hpp"

namespace demo
{
	size_t gCurrIdx = 2;
	vector<shared_ptr<DemoBase>> gpDemos;
	
	void drawGUI() {
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

	void myCallback(const Event& e) {
		int key = *static_pointer_cast<int>(e.mContent);
		if (key == NNKeyMap(ESCAPE)) {
			Utils::setWindowShouldClose(true);
		}
		if (key == NNKeyMap(F5)) {
			for (auto p : gpDemos) {
				p->onInit();
			}
			gpDemos[gCurrIdx]->onEnter();
			printf("All demo's resources have been reloaded.\n");
		}
	}

	int _main() {
		// 初始化
		Utils::init("AA", 800, 600);
		CameraController cc;
		Keyboard::getInstance().onPress.addCallbackFunc(myCallback);
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
		panel.mDrawFunc = drawGUI;
		//
		gpDemos[gCurrIdx]->onEnter();
		// 主循环
		while (!Utils::windowShouldClose()) {
			//
			Utils::pollEvents();
			Utils::update();
			//
			cc.update();
			cc.getCamera()->use();
			NeneCB::getInstance().PerFrame.update(NENE_PER_FRAME_CB_SLOT);
			//
			gpDemos[gCurrIdx]->onLoop(cc.getCamera());
			//
			panel.draw();
			// 
			Utils::swapBuffers();
		}
		//
		gCurrIdx = 0;
		while (!gpDemos.empty()) {
			gpDemos.pop_back();
		}
		// 释放资源
		Utils::terminate();
		//
		return 0;
	}
}
