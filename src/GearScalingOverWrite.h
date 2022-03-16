#pragma once
#include "../CWSDK/cwsdk.h"
#include "utility.h"

extern "C" float GetGearScaling(cube::Item * item, cube::Creature* creature, int base)
{
	IntVector2 region;
	if (creature->entity_data.hostility_type == 0)
	{
		region = creature->entity_data.current_region;
	}
	else
	{
		region = item->region;
	}

	int mod = item->modifier;

	int mod1 = mod ^ (mod << 0x0D);
	int mod2 = mod1 ^ (mod1 >> 0x11);
	int mod3 = mod2 ^ (mod2 << 0x05);

	int effective_rarity = item->GetEffectiveRarity(&region);
	float base_res = ((base * 0.5f) / (float)0x20);
	float mod_modifier = (mod3 / 0x10624DD3) / 7.0f;

	float X = 1.4;
	float Y = base_res + effective_rarity + mod_modifier;

	float result = std::powf(X, Y);

	cube::Game* game = cube::GetGame();
	if (game && creature->entity_data.hostility_type == cube::Creature::EntityBehaviour::Player)
	{
		result *= 0.5 * std::pow(2.7183, 0.2 * GetItemLevel(item)) / 1.21 * std::pow(0.99, 1 + 0.07 * GetItemLevel(item) * GetItemLevel(item));
	}
	return result;
}

extern "C" float GetOtherStatsRe(cube::Item * item, cube::Creature * creature)
{
	IntVector2 region;
	if (creature->entity_data.hostility_type == 0)
	{
		region = creature->entity_data.current_region;
	}
	else
	{
		region = item->region;
	}

	int mod = item->modifier;

	int mod1 = mod ^ (mod << 0x0D);
	int mod2 = mod1 ^ (mod1 >> 0x11);
	int mod3 = mod2 ^ (mod2 << 0x05);

	int effective_rarity = item->GetEffectiveRarity(&region);
	float mod_modifier = (mod3 / 0x10624DD3) / 7.0f;
	float randomizer = 2 * PyroRand(mod3) / 32768;

	float X = 1.2;
	float Y = effective_rarity + mod_modifier;

	float result = std::powf(X, Y);

	cube::Game* game = cube::GetGame();
	if (game && creature->entity_data.hostility_type == cube::Creature::EntityBehaviour::Player)
	{
		result *= 0.01f + 0.0016f * GetItemLevel(item);
		if (result > 0.2f) {
			result = std::log2f(result/0.2f)*0.2f + 0.2f;
			result *= randomizer;
		}
	}
	else {
		result = 0;
	}


	int category = item->category;

	if (category < 3 || category > 9)
	{
		result *= 0;
	}


	return result;
}


extern "C" float GetHasteRe(cube::Item * item, cube::Creature * creature) {
	return GetOtherStatsRe(item, creature) * PyroRand(1 + item->modifier) / 32768;
}

extern "C" float GetRegenRe(cube::Item * item, cube::Creature * creature) {
	return GetOtherStatsRe(item, creature) * PyroRand(2 + item->GetBuyingPrice()) / 32768;
}

extern "C" float GetCritRe(cube::Item * item, cube::Creature * creature) {
	return GetOtherStatsRe(item, creature) * PyroRand(3 + item->GetSellingPrice()) / 32768;
}

extern "C" float GearScaling(float x, float y, cube::Item* item)
{
	float n = std::pow(x, y);
	n *= 0.5f * GetItemLevel(item);
	return n;
}

__attribute__((naked)) void ASM_OverwriteGearScaling() {
	asm(".intel_syntax \n"

		"mov r8, rdi \n"
		"call GearScaling \n"

		//Old code
		"mov rbx, [rsp + 0x30] \n"
		"mov rsi, [rsp + 0x38] \n"
		"add rsp, 0x20 \n"
		"pop rdi \n"
		"retn \n"
	);
}

void Setup_GearScalingOverwrite() {
	//WriteFarJMP(CWOffset(0x109D11), (void*)&ASM_OverwriteGearScaling);
	WriteFarJMP(CWOffset(0x109C50), GetGearScaling);
	WriteFarJMP(CWOffset(0x10A490), GetHasteRe); //haste
	WriteFarJMP(CWOffset(0x109F30), GetRegenRe); //regen
	WriteFarJMP(CWOffset(0x1090F0), GetCritRe); //critical
}