#pragma once
#include "../CWSDK/cwsdk.h"
#include <math.h>

extern "C" int XP_Overwrite(int level)
{
	//return (1050 * level - 50) / (level + 19);
	//return 50 * std::powf(level + 1, 1.3f);
	return 50 + 4*level;
}

__attribute__((naked)) void ASM_XP_Overwrite() {
	asm(".intel_syntax \n"
		// Store rdx
		"push rdx \n"

		// Make space on the stack
		"mov rdx, rsp \n"
		"sub rsp, 0x110 \n"

		// Store xmm registers
		"movaps xmmword ptr [rdx - 0x10], xmm0 \n"
		"movaps xmmword ptr [rdx - 0x20], xmm1 \n"
		"movaps xmmword ptr [rdx - 0x30], xmm2 \n"
		"movaps xmmword ptr [rdx - 0x40], xmm3 \n"
		"movaps xmmword ptr [rdx - 0x50], xmm4 \n"
		"movaps xmmword ptr [rdx - 0x60], xmm5 \n"
		"movaps xmmword ptr [rdx - 0x70], xmm6 \n"
		"movaps xmmword ptr [rdx - 0x80], xmm7 \n"
		"movaps xmmword ptr [rdx - 0x90], xmm8 \n"
		"movaps xmmword ptr [rdx - 0xA0], xmm9 \n"
		"movaps xmmword ptr [rdx - 0xB0], xmm10 \n"
		"movaps xmmword ptr [rdx - 0xC0], xmm11 \n"
		"movaps xmmword ptr [rdx - 0xD0], xmm12 \n"
		"movaps xmmword ptr [rdx - 0xE0], xmm13 \n"
		"movaps xmmword ptr [rdx - 0xF0], xmm14 \n"
		"movaps xmmword ptr [rdx - 0x100], xmm15 \n"
		
		// Store r15
		"push r15 \n"

		PUSH_ALL

		"call XP_Overwrite \n"

		// Store rax to r15 stored on the stack
		"mov [rsp], rax \n"

		POP_ALL

		// Restore rax and pop r15
		"mov rax, r15 \n"
		"pop r15 \n"

		// Restore XMM registers
		"movaps xmm0, xmmword ptr [rdx - 0x10] \n"
		"movaps xmm1, xmmword ptr [rdx - 0x20] \n"
		"movaps xmm2, xmmword ptr [rdx - 0x30] \n"
		"movaps xmm3, xmmword ptr [rdx - 0x40] \n"
		"movaps xmm4, xmmword ptr [rdx - 0x50] \n"
		"movaps xmm5, xmmword ptr [rdx - 0x60] \n"
		"movaps xmm6, xmmword ptr [rdx - 0x70] \n"
		"movaps xmm7, xmmword ptr [rdx - 0x80] \n"
		"movaps xmm8, xmmword ptr [rdx - 0x90] \n"
		"movaps xmm9, xmmword ptr [rdx - 0xA0] \n"
		"movaps xmm10, xmmword ptr [rdx - 0xB0] \n"
		"movaps xmm11, xmmword ptr [rdx - 0xC0] \n"
		"movaps xmm12, xmmword ptr [rdx - 0xD0] \n"
		"movaps xmm13, xmmword ptr [rdx - 0xE0] \n"
		"movaps xmm14, xmmword ptr [rdx - 0xF0] \n"
		"movaps xmm15, xmmword ptr [rdx - 0x100] \n"

		// Restore stack
		"add rsp, 0x110 \n"

		// Restore rdx
		"pop rdx \n"

		"retn \n"
	);
}
void Setup_XP_Overwrite() {
	WriteFarJMP(CWOffset(0X5FA80), (void*)&ASM_XP_Overwrite);
}