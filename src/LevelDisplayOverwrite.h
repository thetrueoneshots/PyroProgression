#pragma once
#include "../CWSDK/cwsdk.h"
#include "utility.h"

void PutText(void* unk, wchar_t* buffer)
{
	((void (*)(void*, wchar_t*))CWOffset(0x486B0))(unk, buffer);
}

extern "C" void OverwriteItemName(cube::Item * item, std::wstring * string)
{
	if (item->category < 3 || item->category > 9)
	{
		return;
	}

	wchar_t buffer[250];
	swprintf_s(buffer, 250, L"LV %d ", GetItemLevel(item));
	*string = buffer + *string;
}

extern "C" void LevelDisplayOverwriteCreature(cube::Creature* creature, void* unk)
{
	wchar_t buffer[250];
	swprintf_s(buffer, 250, L"LV %d ", GetCreatureLevel(creature));
	PutText(unk, buffer);
	return;
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
		"call LevelDisplayOverwriteCreature \n"

		DEREF_JMP(ASM_LevelDisplayOverwrite_jmpback)
	);
}

__attribute__((naked)) void ASM_OverwriteItemName() {
	asm(".intel_syntax \n"
		// rdi: item
		// rsi: wstring
		"mov rdx, rsi \n"
		"lea rcx, [rbp + 0x60] \n"
		"call OverwriteItemName \n"

		// Old code
		"mov rax, rsi \n"
		"mov rcx, [rbp + 0x280] \n"
		"xor rcx, rsp \n"
		"mov rbx, [rsp + 0x3C8] \n"
		"add rsp, 0x390 \n"
		"pop rdi \n"
		"pop rsi \n"
		"pop rbp \n"
		"retn \n"
	);
}

void Setup_LevelDisplayOverwrite() {
	WriteFarJMP(CWOffset(0xB1966), (void*)&ASM_LevelDisplayOverwrite);
	ASM_LevelDisplayOverwrite_jmpback = CWOffset(0xB19AE);

	WriteFarJMP(CWOffset(0x16466C), (void*)&ASM_OverwriteItemName);

	//WriteFarJMP(CWOffset(0x109D11), (void*)&ASM_OverwriteItemName);
}