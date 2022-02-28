#pragma once
#include "../CWSDK/cwsdk.h"
#include "utility.h"

void SetGoldDropValue(int value)
{
	auto address = 0x2A7602;
	auto offset = 0x04;

	WriteByte(CWOffset(address + offset), value & 0xFF);
	WriteByte(CWOffset(address + offset + 1), (value >> 8) & 0xFF);
	WriteByte(CWOffset(address + offset + 2), (value >> 16) & 0xFF);
	WriteByte(CWOffset(address + offset + 3), (value >> 24) & 0xFF);
}

extern "C" void GetGoldDrops(cube::Creature * creature, float* gold)
{
	SetGoldDropValue(GetCreatureLevel(creature));

	*gold *= 10;
	if (*gold <= 0)
	{
		*gold = 1;
	}
}

GETTER_VAR(void*, ASM_OverwriteGoldDrops_jmpback);
__attribute__((naked)) void ASM_OverwriteGoldDrops() {
	asm(".intel_syntax \n"
		
		PUSH_ALL

		// Put result on stack (thanks Chris)
		"movq rax, xmm10 \n"
		"push rax \n"
		"lea rdx, [rsp] \n"

		// Call function
		"mov rcx, rsi \n"

		PREPARE_STACK
		"call GetGoldDrops \n"
		RESTORE_STACK

		// Get result
		"pop rax \n"
		"movq xmm10, rax \n"

		POP_ALL

		// Old code
		"mov ebx, r12d \n"
		"cvttss2si r14, xmm11 \n"

		DEREF_JMP(ASM_OverwriteGoldDrops_jmpback)
	);
}

void Setup_OverwriteGoldDrops() {
	WriteFarJMP(CWOffset(0x2A752C), ASM_OverwriteGoldDrops);
	ASM_OverwriteGoldDrops_jmpback = CWOffset(0x2A7540);
}