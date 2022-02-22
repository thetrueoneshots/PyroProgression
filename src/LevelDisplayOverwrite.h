#pragma once
#include "../CWSDK/cwsdk.h"
#include "utility.h"

extern "C" void LevelDisplayOverwrite(cube::Creature* creature, void* unk)
{
	wchar_t buffer[250];
	swprintf_s(buffer, 250, L"LV %d ", GetLevel(creature));
	((void (*)(void*, wchar_t*))CWOffset(0x486B0))(unk, buffer);
}

extern "C" void sub_336F0(void* a1, int a2, int a3)
{
	((void (*)(void*, int, int))CWOffset(0x336F0))(a1, a2, a3);
}

GETTER_VAR(void*, ASM_LevelDisplayOverwrite_jmpback);
__attribute__((naked)) void ASM_LevelDisplayOverwrite() {
	asm(".intel_syntax \n"
		"xor r8d, r8d \n"
		"mov dl, 0x1 \n"
		"lea rcx, [rbp + 0x78] \n"
		"call sub_336F0 \n"

		"lea rdx, [rbp - 0x80] \n"
		"mov rcx, [r14] \n"
		"call LevelDisplayOverwrite \n"

		DEREF_JMP(ASM_LevelDisplayOverwrite_jmpback)
	);
}
void Setup_LevelDisplayOverwrite() {
	WriteFarJMP(CWOffset(0xB1966), (void*)&ASM_LevelDisplayOverwrite);
	ASM_LevelDisplayOverwrite_jmpback = CWOffset(0xB19AE);
}