#include "Antidebug.h"
#include <Windows.h>

namespace Antidebug {

/*
 * Should appear to a novice reverse engineer to be doing
 * some useful work. It'll actually crash and cause the debugger
 * to catch the exception. If no debugger is attached, control
 * will be handed to the __except block.
 */

void debuggerTrap2(int a) {
	debuggerTrap2(5);
	MessageBox(NULL, "SS", "SSSS", 0);
}

void debuggerTrap() {
	__try {
		char* deceptive_string = "ui/tags/field_of_view";
		void* random_pointer = reinterpret_cast<void*>(0xFFFFFF);

		__asm {
			__emit 0xF1
			call debuggerTrap
			mov random_pointer, esp
			shr random_pointer, 5
			xor random_pointer, ebp
			shl random_pointer, 32
		}

		memcpy((void*)random_pointer, deceptive_string, sizeof(deceptive_string));
	} __except(EXCEPTION_EXECUTE_HANDLER) {
		OutputDebugString("mm");
	}
}

/*
 * Pretty useless if the reverser just passes the exception
 * to Halo.
 */
inline void debuggerDetect() {
	int flag = 0;

	__try {
		__asm {
			__emit 0xF1;
		}
	} __except(EXCEPTION_EXECUTE_HANDLER) {
		flag = 1;
	}

	if(flag == 0) {
		//debugger found
	}
}

}