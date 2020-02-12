/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_GL

#include "GUI.h"
#include "Utils.h"
#include "ImGui/imgui.h"
#include "ImGui/imconfig.h"
#include "ImGui/imgui_impl_glfw_gl3.h"

using namespace std;

int GUI::sInsNum = 0;

GUI::GUI() {
	//
	if (sInsNum == 0) {
		ImGui_ImplGlfwGL3_Init(Utils::GetWindow(), false);
		ImGui::GetIO().IniFilename = nullptr;
		++sInsNum;
	}
	//
	mDrawFunc = sDefaultDrawFunc;
}

GUI::~GUI() {
	--sInsNum;
	if (sInsNum == 0) {
		ImGui_ImplGlfwGL3_Shutdown();
	}
}

std::shared_ptr<GUI> GUI::Create()
{
	GUI* ret = new GUI();
	return std::shared_ptr<GUI>(ret);
}

void GUI::sDefaultDrawFunc() {
	// 大小
	ImGui::Begin("Status", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos(ImVec2(10, 10));
	ImGui::SetWindowSize(ImVec2(240, 180));
	// 性能
	ImGui::TextColored(ImVec4(0.411f, 1.0f, 0.956f, 1.0), "Info:");
	ImGui::Text("%.2f ms/frame (%.0f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

void GUI::Draw() {
	//
	static GLint oldPolygonMode;
	glGetIntegerv(GL_POLYGON_MODE, &oldPolygonMode);
	//
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//!TODO: Set render state for just gui
	ImGui_ImplGlfwGL3_NewFrame();
	mDrawFunc();
	ImGui::End();
	ImGui::Render();
	//!TODO: Reset render state for just gui
	glPolygonMode(GL_FRONT_AND_BACK, oldPolygonMode);
}

#endif