#pragma once
#include "../CWSDK/cwsdk.h"
#include "utility.h"

extern "C" void RegionTextDrawOverwrite(plasma::Node* node, std::wstring* string)
{
	auto distance = GetRegionDistance(cube::GetGame()->GetPlayer()->entity_data.current_region);
	long long upper = (distance + 1) * LEVELS_PER_REGION;
	long long lower = distance * LEVELS_PER_REGION + 1;
	wchar_t buffer[250];

	if (upper < 1000)
	{
		swprintf_s(buffer, 250, L"LV.%d-%d ", lower, upper);
	}
	else if (upper < 1000000)
	{
		swprintf_s(buffer, 250, L"LV.%dK ", (upper / 1000));
	}
	else
	{
		swprintf_s(buffer, 250, L"LV.%dM ", (upper / 1000000));
	}
	
	
	*string = buffer + *string;
	node->SetText(string);
	return;
}

GETTER_VAR(void*, ASM_RegionTextDrawOverwrite_jmpback);
GETTER_VAR(void*, ASM_RegionTextDrawOverwrite_bail);
__attribute__((naked)) void ASM_RegionTextDrawOverwrite() {
	asm(".intel_syntax \n"
		
		// String is already set in rdx
		"mov rcx, [r13 + 0x2D0] \n"
		"call RegionTextDrawOverwrite \n"

		"test esi, esi \n"
		"jz 1f \n"

		DEREF_JMP(ASM_RegionTextDrawOverwrite_jmpback)

		"1: \n"

		DEREF_JMP(ASM_RegionTextDrawOverwrite_bail)
	);
}

void Setup_RegionTextDrawOverwrite() {
	WriteFarJMP(CWOffset(0xABA58), (void*)&ASM_RegionTextDrawOverwrite);
	ASM_RegionTextDrawOverwrite_jmpback = CWOffset(0xABA68);
	ASM_RegionTextDrawOverwrite_bail = CWOffset(0xABA8C);
}