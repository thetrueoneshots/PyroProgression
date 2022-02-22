#pragma once
#include "../CWSDK/cwsdk.h"

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