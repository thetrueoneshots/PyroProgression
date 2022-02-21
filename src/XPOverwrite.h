#pragma once
#define MODLOADER 0
#include "../CWSDK/cwsdk.h"

void WriteFarJMP(void* source, void* destination) {
	DWORD dwOldProtection;
	VirtualProtect(source, 14, PAGE_EXECUTE_READWRITE, &dwOldProtection);
	char* location = (char*)source;

	// Far jump
	*((UINT16*)&location[0]) = 0x25FF;

	// mode
	*((UINT32*)&location[2]) = 0x00000000;

	*((UINT64*)&location[6]) = (UINT64)destination;

	VirtualProtect(location, 14, dwOldProtection, &dwOldProtection);
}

extern "C" int XP_Overwrite(int level)
{
	return (1050 * level - 50) / (level + 19);
}

__attribute__((naked)) void ASM_XP_Overwrite() {
	asm(".intel_syntax \n"
		"call XP_Overwrite \n"
		"retn \n"
	);
}
void Setup_XP_Overwrite() {
	WriteFarJMP(CWOffset(0X5FA80), (void*)&ASM_XP_Overwrite);
}