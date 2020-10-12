/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_GL

#include "Utils.h"
#include "UserInterface.h"

#include "ImGui/imgui.h"
#include "ImGui/imconfig.h"
#include "ImGui/imgui_impl_glfw_gl3.h"

using namespace std;

/** Static Memeber Initialization >>> */

int UserInterface::s_instance_num = 0;

function<void()> UserInterface::s_default_draw_function = []() {
	ImGui::Begin("Status", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	{
		//
		ImGui::SetWindowPos(ImVec2(10, 10));
		ImGui::SetWindowSize(ImVec2(240, 180));
		//
		ImGui::TextColored(ImVec4(0.411f, 1.0f, 0.956f, 1.0), "Info:");
		ImGui::Text("%.2f ms/frame (%.0f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	ImGui::End();
};

/** Static Memeber Initialization <<< */

UserInterface::~UserInterface()
{
	--s_instance_num;
	if (s_instance_num == 0)
	{
		ImGui_ImplGlfwGL3_Shutdown();
	}
}

shared_ptr<UserInterface> UserInterface::Create(function<void()> newfunc)
{
	if (s_instance_num == 0)
	{
		ImGui_ImplGlfwGL3_Init(Utils::GetWindow(), false);
		ImGui::GetIO().IniFilename = nullptr;
		++s_instance_num;
	}
	//
	UserInterface* result = new UserInterface();
	if (newfunc != nullptr)
	{
		result->m_draw_function = newfunc;
	}
	else
	{
		result->m_draw_function = s_default_draw_function;
	}
	return shared_ptr<UserInterface>(result);
}

void UserInterface::Draw() {
	//
	static GLint oldPolygonMode;
	glGetIntegerv(GL_POLYGON_MODE, &oldPolygonMode);
	//
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//!TODO: Set render state for just gui
	ImGui_ImplGlfwGL3_NewFrame();
	m_draw_function();
	//
	ImGui::Render();
	//!TODO: Reset render state for just gui
	glPolygonMode(GL_FRONT_AND_BACK, oldPolygonMode);
}

#endif