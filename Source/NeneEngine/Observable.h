/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef OBSERVABLE_H
#define OBSERVABLE_H

#include "Observer.h"

#include <list>
#include <vector>
#include <functional>

//
//    Observable: Subject that can be observe
//

class Observable {
public:
	// 构造
	Observable();
	// 析构
	~Observable();
	// 增加观察者
	void AddObserver(Observer* observer);
	// 删除观察者
	void DelObserver(Observer* observer);
	// 增加回调函数
	NNInt AddCallbackFunc(std::function<void(std::shared_ptr<BaseEvent> eve)> func);
	// 删除回调函数
	void DelCallbackFunc(const NNInt& funcID);
	// 通知所有观察者
	virtual void Notify(std::shared_ptr<BaseEvent> eve);
protected:
	// 指向观察者们的链表
	std::list<Observer*> mObservers;
	// 指向回调函数的链表
	std::vector<std::function<void(std::shared_ptr<BaseEvent> eve)>> mCallbackFuncs;
};

#endif // OBSERVABLE_H