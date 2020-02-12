/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef GUI_H
#define GUI_H

#include <memory>
#include <functional>

//
//    GUI: 
//
class GUI {
public:
	//
	static std::shared_ptr<GUI> Create();
	//
	virtual ~GUI();
	//
	void Draw();
	inline void SetDrawFunc(std::function<void()> func) { mDrawFunc = func; }
protected:
	std::function<void()> mDrawFunc;
	static int sInsNum;
	static void sDefaultDrawFunc();
protected:
	GUI();
	GUI(const GUI& rhs) = delete;
	GUI& operator=(const GUI& rhs) = delete;
};

#endif // GUI_H