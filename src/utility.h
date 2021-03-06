#pragma once
#include "cwsdk.h"

#define LEVELS_PER_REGION 5
#define LEVEL_EQUIPMENT_CAP 0

int GetRegionDistance(IntVector2 region);
int GetItemLevel(cube::Item* item);
int GetCreatureLevel(cube::Creature* creature);
int GetLevelVariation(long long modifier, int range);
void SetEquipmentRegion(cube::Creature* creature, IntVector2 region);

unsigned long long PyroRand(unsigned long long seed);