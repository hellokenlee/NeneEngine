/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_DX

#include "Utils.h"
#include "GUI.h"
#include "ImGui/imgui.h"
#include "ImGui/imconfig.h"
#include "ImGui/imgui_impl_dx11.h"

using namespace std;

int GUI::sInsNum = 0;

void GUI::sDefaultDrawFunc() {
	// 大小
	ImGui::Begin("Status", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos(ImVec2(10, 10));
	ImGui::SetWindowSize(ImVec2(240, 180));
	// 性能
	ImGui::TextColored(ImVec4(0.411f, 1.0f, 0.956f, 1.0), "Info:");
	ImGui::Text("%.2f ms/frame (%.0f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

GUI::GUI() {
	//
	if (sInsNum == 0) {
		ImGui_ImplDX11_Init(Utils::GetWindow(), Utils::getDevice(), Utils::getContext());
		ImGui::GetIO().IniFilename = nullptr;
		++sInsNum;
	}
	//
	mDrawFunc = sDefaultDrawFunc;
}

GUI::~GUI() {
	--sInsNum;
	if (sInsNum == 0) {
		ImGui_ImplDX11_Shutdown();
	}
}

void GUI::Draw() {
	//!TODO: Set render state for just gui
	if (!Utils::windowShouldClose()) {
		auto a = Utils::GetWindow();
		ImGui_ImplDX11_NewFrame();
		mDrawFunc();
		ImGui::End();
		ImGui::Render();
	}
	//!TODO: Reset render state for just gui
}

#endif