/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef OBSERVER_H
#define OBSERVER_H

#include "Event.h"

//
//    Observer: An Observer Interface of Observer Pattern
//

class Observer : public std::enable_shared_from_this<Observer> {
public:
	// 析构
	virtual ~Observer();
	// 事件处理函数
	virtual void OnNotify(std::shared_ptr<BaseEvent> eve) = 0;
};

#endif // OBSERVER_H