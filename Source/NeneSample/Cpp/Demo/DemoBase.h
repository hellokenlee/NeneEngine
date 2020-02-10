#pragma once

#ifndef DEMO_BASE_H
#define DEMO_BASE_H

#include <string>
#include "../Source/CameraController.h"

enum DemoStatus {
	STATUS_HIDE,
	STATUS_SHOW,
	STATUS_SPLIT,

	STATUS_COUNT
};

class DemoBase {
public:
	// Init all your `unreloadable` resources here
	DemoBase(const std::string& name="Default") : mName(name), mStatus(STATUS_HIDE) {};
	// Release all your resources
	virtual ~DemoBase() {};

	// Init all your reloadable here
	virtual void onInit() = 0;
	// Called by main loop
	virtual void onLoop(shared_ptr<Camera> camera) = 0;
	//
	virtual void onEnter() = 0;
	//
	virtual void onLeave() = 0;
public:
	std::string mName;
	DemoStatus mStatus;
};


#endif // DEMO_BASE_H