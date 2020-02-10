/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/

#include "NeneCB.h"

NeneCB::NeneCB() {}

NeneCB& NeneCB::Instance() {
	static NeneCB instance;
	return instance;
}