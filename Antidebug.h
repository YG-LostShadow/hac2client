#pragma once

#define ENCRYPT_BEGIN {					\
	DWORD old;							\
	__asm push esi						\
	__asm push eax						\
	__asm mov eax, FunctionEnd			\
	__asm sub eax, FunctionBegin		\
	__asm lea esi, old					\
	__asm push esi 						\
	__asm push PAGE_EXECUTE_READWRITE	\
	__asm push eax						\
	__asm push FunctionBegin			\
	__asm lea eax, VirtualProtect		\
	__asm call [eax]					\
	__asm lea esi, [FunctionBegin + 4]	\
	__asm mov al, 0xFF					\
	__asm DoDecrypt:					\
	__asm xor [esi], al					\
	__asm inc esi						\
	__asm cmp esi, FunctionEnd			\
	__asm jne DoDecrypt					\
	__asm pop eax						\
	__asm pop esi						\
	__asm FunctionBegin:				\
	__asm __emit 0xF1					\
	__asm __emit 0xF1					\
	__asm __emit 0xF1					\
	__asm __emit 0xF1					\
}

#define ENCRYPT_END {					\
	__asm FunctionEnd:					\
	__asm __emit 0xF1					\
	__asm __emit 0xF1					\
	__asm __emit 0xF1					\
	__asm __emit 0xF1					\
	__asm push esi						\
	__asm push eax						\
	__asm mov al, 0xFF					\
	__asm lea esi, [FunctionBegin + 4]	\
	__asm DoEncrypt:					\
	__asm xor [esi], al					\
	__asm inc esi						\
	__asm cmp esi, FunctionEnd			\
	__asm jne DoEncrypt					\
	__asm pop eax						\
	__asm pop esi						\
}

namespace Antidebug {
	void debuggerTrap();
}