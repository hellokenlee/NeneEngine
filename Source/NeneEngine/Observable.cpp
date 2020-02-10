/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#include "Observable.h"

using namespace std;

Observable::Observable() {
	mObservers.clear();
	mCallbackFuncs.clear();
}

Observable::~Observable() {

}

NNInt Observable::AddCallbackFunc(std::function<void(shared_ptr<BaseEvent> eve)> func) {
	// 
	int id = -1;
	for (size_t i = 0; i < mCallbackFuncs.size(); ++i) {
		if (mCallbackFuncs[i] == nullptr) {
			id = (int)i;
			break;
		}
	}
	//
	if (id == -1) {
		id = (int)mCallbackFuncs.size();
		mCallbackFuncs.push_back(nullptr);
	}
	mCallbackFuncs[id] = func;
	return id;
}

void Observable::DelCallbackFunc(const NNInt& funcID) {
	mCallbackFuncs[funcID] = nullptr;
}

void Observable::AddObserver(Observer* observer) {
	mObservers.push_back(observer);
}

void Observable::DelObserver(Observer* observer) {
	for (auto it = mObservers.begin(); it != mObservers.end(); ++it) {
		if ((*it) == observer) {
			it = mObservers.erase(it);
			break;
		}
	}
}

void Observable::Notify(shared_ptr<BaseEvent> eve) {
	//
	for (auto it = mObservers.begin(); it != mObservers.end(); ++it) {
		(*it)->OnNotify(eve);
	}
	//
	for (auto& callback : mCallbackFuncs) {
		callback(eve);
	}
}