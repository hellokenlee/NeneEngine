/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef GUI_H
#define GUI_H

#include "ImGui/imgui.h"
#include <functional>

//
//    GUI: 
//
class GUI {
public:
	//
	GUI();
	~GUI();
	//
	void Draw();
public:
	std::function<void()> mDrawFunc;
private:
	static int sInsNum;
	static void sDefaultDrawFunc();
};

#endif // GUI_H