/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef GUI_H
#define GUI_H

#include <memory>
#include <functional>
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

//
//    UserInterface: 
//
class UserInterface 
{
public:
	//
	~UserInterface();
	static std::shared_ptr<UserInterface> Create(std::function<void()> newfunc=nullptr);
	//
	void Draw();
	//
	void SetDrawFunction(std::function<void()> newfunc) { m_draw_function = newfunc; };
protected:
	std::function<void()> m_draw_function;
private:
	//
	UserInterface() = default;
	UserInterface(const UserInterface& rhs) = delete;
	UserInterface& operator=(const UserInterface& rhs) = delete;
	//
	static int s_instance_num;
	static std::function<void()> s_default_draw_function;
};

#endif // GUI_H