#pragma once

#include "EventSubscriber.h"
#include <vector>
#include <WinDef.h>
#include <map>
#include <memory>

class Anticheat : public EventSubscriber {
	DWORD run();
	void processQueue();
	bool exit;
	EventSubscriber* dispatcher;
	void getAsyncKeyStatePatch();
	std::map<EVENTS, void (Anticheat::*)(std::shared_ptr<Event>)> functions;
	void generateMap();
	void debuggerTrap();
	void threadHandle(std::shared_ptr<Event> event);
	void pingRespond(std::shared_ptr<Event> event);

public:
	void registerDispatcher(EventSubscriber* dispatcher);
	Anticheat();
};