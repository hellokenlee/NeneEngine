/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef EVENT_H
#define EVENT_H

#include "Types.h"

#include <string>
#include <memory>

//
//    Event: classure passed from ovservables to observer
//

class BaseEvent 
{
public:
	// 构造函数
	BaseEvent(const NNUInt& code = 0) : mCode(code) {}
	virtual ~BaseEvent() {}
	// 事件代号
	NNUInt mCode;
};


class KeyboardEvent : public BaseEvent
{
public:
	// 构造函数
	KeyboardEvent(const NNInt& key, const NNUInt code) :  BaseEvent(code), mKey(key) {}
	// 按键编号
	NNInt mKey;
};


class KeyPressEvent : public KeyboardEvent
{
public:
	KeyPressEvent(const NNInt& key) : KeyboardEvent(key, ON_KEY_PRESS) {}
};


class KeyRepeatEvent : public KeyboardEvent
{
public:
	KeyRepeatEvent(const NNInt& key) : KeyboardEvent(key, ON_KEY_REPEAT) {}
};


class KeyReleaseEvent : public KeyboardEvent
{
public:
	KeyReleaseEvent(const NNInt& key) : KeyboardEvent(key, ON_KEY_RELEASE) {}
};


class MouseMoveEvent : public BaseEvent
{
public:
	//
	MouseMoveEvent(const NNFloat& dx, const NNFloat& dy) : BaseEvent(ON_MOUSE_MOVE), mdx(dx), mdy(dy) {}
	//
	NNFloat mdx, mdy;
};


class MouseScrollEvent : public BaseEvent
{
public:
	//
	MouseScrollEvent(const NNFloat& dx, const NNFloat& dy) : BaseEvent(ON_MOUSE_SCROLL), mdx(dx), mdy(dy) {}
	//
	NNFloat mdx, mdy;
};


#endif // EVENT_H