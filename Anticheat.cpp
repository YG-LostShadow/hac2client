#include "Anticheat.h"
#include "Antidebug.h"
#include "DebugHelper.h"
#include <cstdint>

void Anticheat::threadHandle(std::shared_ptr<Event> event) {
	std::shared_ptr<ThreadHandleEvent> thread(new ThreadHandleEvent(
		OpenThread(THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME, FALSE,
			GetThreadId(GetCurrentThread()))));
	OutputDebugString("Sent handle!");
	broadcast(thread);	
}

void Anticheat::pingRespond(std::shared_ptr<Event> event) {
	std::shared_ptr<Event> response(std::make_shared<Event>(PONG_ANTICHEAT_THREAD));
	OutputDebugString("Anticheat responding!");
	broadcast(response);
}

void Anticheat::generateMap() {
	functions[POST_MAP_LOAD] = &Anticheat::pingRespond;
	functions[PING_ANTICHEAT_THREAD] = &Anticheat::pingRespond;
	functions[REQUEST_ANTICHEAT_THREAD_HANDLE] = &Anticheat::threadHandle;
	
	//ENCRYPT_BEGIN;
	for(auto iter = functions.begin(); iter != functions.end(); ++iter) {
		int eventType = iter->first;
		auto& function = iter->second;
		__asm {
			push eax
			mov eax, eventType
			xor DWORD PTR [function], eax
			pop eax
		}
	}
	//ENCRYPT_END;
}

Anticheat::Anticheat() {
	generateMap();
	getAsyncKeyStatePatch();
}

DWORD Anticheat::run() {
	exit = false;

	while(!exit) {
		WaitForSingleObject(eventSem, INFINITE);
		processQueue();
	}

	return 0;
}

void Anticheat::processQueue() {
	std::shared_ptr<Event> event;

	while(!events.empty()) {
		if(!events.try_pop(event)) {
			return;
		}

		void (Anticheat::*func)(std::shared_ptr<Event>) = functions[event->type];
		int eventType = event->type;

		//Antidebug::debuggerTrap();
		//ENCRYPT_BEGIN;
		__asm {
			push eax
			//mov eax, eventType
			//xor DWORD PTR [func], eax
			pop eax
		}
		//ENCRYPT_END;

		(this->*func)(event);
	}
}

void Anticheat::registerDispatcher(EventSubscriber* dispatcher) {
	this->dispatcher = dispatcher;
	std::vector<EVENTS> events;
	//ENCRYPT_BEGIN;
	events.emplace_back(POST_MAP_LOAD);
	events.emplace_back(REQUEST_ANTICHEAT_THREAD_HANDLE);
	events.emplace_back(PING_ANTICHEAT_THREAD);
	//ENCRYPT_END;
	dispatcher->subscribe(this, events);
}

void Anticheat::getAsyncKeyStatePatch() {
	DWORD old;		
	uint8_t* func = reinterpret_cast<uint8_t*>(GetAsyncKeyState) - 5;
	VirtualProtect(func, 16, PAGE_EXECUTE_READWRITE, &old);
	/*ENCRYPT_BEGIN;
	func[3] ^= 0x94;
	func[0] ^= 0xA1;
	func[1] ^= 0x50;
	func[4] ^= 0x90;
	func[2] ^= 0x52;
	func[5] = 0xEB;
	func[6] = 0xF9;
	ENCRYPT_END;*/;
}