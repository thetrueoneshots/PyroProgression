#include "utility.h"

int GetLevel(cube::Creature* creature)
{
	cube::Game* game = cube::GetGame();
	if (!game)
	{
		return creature->entity_data.level;
	}

	IntVector2 base_region = game->GetPlayer()->entity_data.equipment.unk_item.region;
	IntVector2 current_region = creature->entity_data.current_region;

	int xDiv = std::abs(base_region.x - current_region.x);
	int yDiv = std::abs(base_region.y - current_region.y);
	int distance = std::max<int>(xDiv, yDiv);

	switch (creature->entity_data.hostility_type)
	{
	case cube::Creature::EntityBehaviour::Player:
	case cube::Creature::EntityBehaviour::Pet:
		return creature->entity_data.level;
	default:
		return creature->entity_data.level + 1 + distance;
	}
}
