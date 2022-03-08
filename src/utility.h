#pragma once
#include "cwsdk.h"

int GetRegionDistance(IntVector2 region);
int GetItemLevel(cube::Item* item);
int GetCreatureLevel(cube::Creature* creature);
int GetLevelVariation(long long modifier, int range);

unsigned long long PyroRand(unsigned long long seed);