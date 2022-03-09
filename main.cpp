#include "main.h"
#include <map>

// Public variables.
// For example: int number = 10;

// Includes for the self written hooks.
// For example: #include "src/hooks/a_hook.h" 
#include "src/utility.h"
#include "src/XPOverwrite.h"
#include "src/LevelDisplayOverwrite.h"
#include "src/GearScalingOverWrite.h"
#include "src/GoldDropOverWrite.h"

void OnLevelUp(cube::Game* game, cube::Creature* creature)
{
	FloatRGBA purple(0.65f, 0.40f, 1.0f, 1.0f);
	cube::TextFX xpText = cube::TextFX();

	// Play sound effect
	game->PlaySoundEffect(cube::Game::SoundEffect::sound_level_up);

	// Set levelup textFX
	xpText.position = creature->entity_data.position;
	xpText.animation_length = 3000;
	xpText.distance_to_fall = -500;
	xpText.color = purple;
	xpText.size = 64;
	xpText.offset_2d = FloatVector2(0, 0);
	xpText.text = L"LEVEL UP!\n";
	xpText.field_60 = 0;
	game->textfx_list.push_back(xpText);

	// Print levelup in chat
	game->PrintMessage(L"LEVEL UP!\n", &purple);

	// Restore health
	creature->entity_data.HP = creature->GetMaxHP();
}

/* Mod class containing all the functions for the mod.
*/
class Mod : GenericMod {

	std::map<int, float> m_PlayerScaling;
	std::map<int, float> m_CreatureScaling;

	enum STAT_TYPE : int {
		ARMOR = 0,
		CRIT,
		ATK_POWER,
		SPELL_POWER,
		HASTE,
		HEALTH,
		RESISTANCE,
		STAMINA,
		MANA
	};

	std::vector<cube::TextFX> m_FXList;
	/* Hook for the chat function. Triggers when a user sends something in the chat.
	 * @param	{std::wstring*} message
	 * @return	{int}
	*/
	virtual int OnChat(std::wstring* message) override {
		return 0;
	}

	virtual void OnGameUpdate(cube::Game* game) override {
		if (m_FXList.size() > 0)
		{
			game->textfx_list.push_back(m_FXList.at(0));
			m_FXList.erase(m_FXList.begin());
		}
	}

	/* Function hook that gets called every game tick.
	 * @param	{cube::Game*} game
	 * @return	{void}
	*/
	virtual void OnGameTick(cube::Game* game) override {
		static bool init = false;
		cube::Creature* player = game->GetPlayer();
		cube::Creature::EntityData* entity_data = &player->entity_data;

		if (!init)
		{
			init = true;

			// Disable old leveling system
			for (int i = 0; i < 10; i++)
			{
				WriteByte(CWOffset(0x6688F + i), 0x90);
			}
			for (int i = 0; i < 6; i++)
			{
				WriteByte(CWOffset(0x668FA + i), 0x90);
			}
		}

		// Show and move xp bar and level info
		plasma::Node* node = game->gui.levelinfo_node;
		node->SetVisibility(true);
		node->Translate(game->width / 2, game->height, -200, -100);

		// Handle levelups
		while (entity_data->XP >= player->GetXPForLevelup())
		{
			entity_data->XP -= player->GetXPForLevelup();
			entity_data->level += 1;

			OnLevelUp(game, player);
		}

		// Set starting region if not set
		// Todo: Change if world settings are implemented
		cube::Item* storage = &entity_data->equipment.unk_item;
		if (storage->modifier == 0)
		{
			storage->modifier = 1;
			storage->region = entity_data->current_region;
		}

		return;
	}

	virtual void OnCreatureDeath(cube::Game* game, cube::Creature* creature, cube::Creature* attacker) override {
		if (creature == nullptr)
		{
			return;
		}

		// Set correct position for items to drop
		if (creature->entity_data.current_region == IntVector2(0, 0))
		{
			// Guessing that entity_data.some_position is the initial spawning position
			creature->entity_data.some_position = creature->entity_data.position;
		}

		if (attacker == nullptr)
		{
			return;
		}

		// Skip on mage bubbles
		if (creature->entity_data.race == 285)
		{
			return;
		}

		cube::Creature* player = game->GetPlayer();
		if (player->id == attacker->id || player->pet_id == attacker->id)
		{
			FloatRGBA purple(0.65f, 0.40f, 1.0f, 1.0f);
			int xp_gain = GetCreatureLevel(creature) * (creature->entity_data.level + 1);

			if ((creature->entity_data.appearance.flags2 & (1 << (int)cube::Creature::AppearanceModifiers::IsBoss)) != 0)
			{
				xp_gain *= 10;
			}

			if ((creature->entity_data.appearance.flags2 & (1 << (int)cube::Creature::AppearanceModifiers::IsNamedBoss)) != 0)
			{
				xp_gain *= 10;
			}

			if ((creature->entity_data.appearance.flags2 & (1 << (int)cube::Creature::AppearanceModifiers::IsMiniBoss)) != 0)
			{
				xp_gain *= 10;
			}

			wchar_t buffer[250];
			swprintf_s(buffer, 250, L"You gain %d xp.\n", xp_gain);
			game->PrintMessage(buffer, &purple);

			cube::TextFX xpText = cube::TextFX();
			xpText.position = player->entity_data.position + LongVector3(std::rand() % (cube::DOTS_PER_BLOCK * 50) , std::rand() % (cube::DOTS_PER_BLOCK * 50), std::rand() % (cube::DOTS_PER_BLOCK * 50));
			xpText.animation_length = 3000;
			xpText.distance_to_fall = -100;
			xpText.color = purple;
			xpText.size = 32;
			xpText.offset_2d = FloatVector2(-50, -100);
			xpText.text = std::wstring(L"+") + std::to_wstring(xp_gain) + std::wstring(L" XP");;
			xpText.field_60 = 0;

			for (int i = 0; i < 4; i++)
			{
				m_FXList.push_back(xpText);
			}

			player->entity_data.XP += xp_gain;
		}
	}

	virtual void OnGetItemBuyingPrice(cube::Item* item, int* price) override {
		int category = item->category;

		if (category >= 3 && category <= 9)
		{
			*price *= GetItemLevel(item);
		}

		if (category == 15)
		{
			*price *= GetItemLevel(item);
		}
	}

	virtual void OnCreatureCanEquipItem(cube::Creature* creature, cube::Item* item, bool* equipable) override
	{
		if (creature->entity_data.hostility_type != cube::Creature::EntityBehaviour::Player)
		{
			return;
		}

		if (creature->entity_data.level + LEVEL_EQUIPMENT_CAP >= GetItemLevel(item))
		{
			return;
		}

		*equipable = false;
	}

	virtual void OnClassCanWearItem(cube::Item* item, int classType, bool* wearable) override
	{
		cube::Game* game = cube::GetGame();

		if (!game)
		{
			return;
		}

		this->OnCreatureCanEquipItem(game->GetPlayer(), item, wearable);
	}

	/* Function hook that gets called on intialization of cubeworld.
	 * [Note]:	cube::GetGame() is not yet available here!!
	 * @return	{void}
	*/
	virtual void Initialize() override {
		Setup_OverwriteGoldDrops();
		Setup_XP_Overwrite();
		Setup_LevelDisplayOverwrite();
		Setup_GearScalingOverwrite();

		// ##### PLAYER ######
		// Defense
		m_PlayerScaling.insert_or_assign(STAT_TYPE::HEALTH, 5);
		m_PlayerScaling.insert_or_assign(STAT_TYPE::ARMOR, 0.5f);
		m_PlayerScaling.insert_or_assign(STAT_TYPE::RESISTANCE, 0.5f);
		
		// Offense
		m_PlayerScaling.insert_or_assign(STAT_TYPE::ATK_POWER, 1);
		m_PlayerScaling.insert_or_assign(STAT_TYPE::SPELL_POWER, 1);
		m_PlayerScaling.insert_or_assign(STAT_TYPE::CRIT, 0.0001f);
		m_PlayerScaling.insert_or_assign(STAT_TYPE::HASTE, 0.0001f);
		
		// Utility
		m_PlayerScaling.insert_or_assign(STAT_TYPE::STAMINA, 0.05f);
		m_PlayerScaling.insert_or_assign(STAT_TYPE::MANA, 0.05f);

		// ##### CREATURE ######
		// Defense
		m_CreatureScaling.insert_or_assign(STAT_TYPE::HEALTH, 20);
		m_CreatureScaling.insert_or_assign(STAT_TYPE::ARMOR, 0);
		m_CreatureScaling.insert_or_assign(STAT_TYPE::RESISTANCE, 0);

		// Offense
		m_CreatureScaling.insert_or_assign(STAT_TYPE::ATK_POWER, 1);
		m_CreatureScaling.insert_or_assign(STAT_TYPE::SPELL_POWER, 1);
		m_CreatureScaling.insert_or_assign(STAT_TYPE::CRIT, 0);
		m_CreatureScaling.insert_or_assign(STAT_TYPE::HASTE, 0);

		// Utility
		m_PlayerScaling.insert_or_assign(STAT_TYPE::STAMINA, 0);
		m_PlayerScaling.insert_or_assign(STAT_TYPE::MANA, 0);

		return;
	}

	virtual void OnItemGetGoldBagValue(cube::Item* item, int* gold) override {
		cube::Game* game = cube::GetGame();
		if (!game)
		{
			return;
		}
		*gold = 100 * (GetRegionDistance(game->GetPlayer()->entity_data.current_region) + 1);
	}

	void ApplyStatBuff(cube::Creature* creature, float* stat, STAT_TYPE type)
	{
		cube::Game* game = cube::GetGame();
		if (!game)
		{
			return;
		}

		if (creature->id == game->GetPlayer()->id)
		{
			*stat += m_PlayerScaling.at(type) * creature->entity_data.level;
		}
	}

	void ApplyCreatureBuff(cube::Creature* creature, float* stat, STAT_TYPE type)
	{
		if (creature->entity_data.hostility_type != cube::Creature::EntityBehaviour::Player &&
			creature->entity_data.hostility_type != cube::Creature::EntityBehaviour::Pet)
		{
			*stat /= creature->entity_data.level + 1;
			*stat += m_CreatureScaling.at(type) * (GetCreatureLevel(creature) - LEVELS_PER_REGION / 2);
			*stat *= creature->entity_data.level + 1;
		}
	}

	virtual void OnCreatureArmorCalculated(cube::Creature* creature, float* armor) override {
		ApplyStatBuff(creature, armor, STAT_TYPE::ARMOR);
		ApplyCreatureBuff(creature, armor, STAT_TYPE::ARMOR);
	}

	virtual void OnCreatureCriticalCalculated(cube::Creature* creature, float* critical) override {
		ApplyStatBuff(creature, critical, STAT_TYPE::CRIT);
		ApplyCreatureBuff(creature, critical, STAT_TYPE::CRIT);
	}

	virtual void OnCreatureAttackPowerCalculated(cube::Creature* creature, float* power) override {
		ApplyStatBuff(creature, power, STAT_TYPE::ATK_POWER);
		ApplyCreatureBuff(creature, power, STAT_TYPE::ATK_POWER);
	}

	virtual void OnCreatureSpellPowerCalculated(cube::Creature* creature, float* power) override {
		ApplyStatBuff(creature, power, STAT_TYPE::SPELL_POWER);
		ApplyCreatureBuff(creature, power, STAT_TYPE::SPELL_POWER);
	}

	virtual void OnCreatureHasteCalculated(cube::Creature* creature, float* haste) override {
		ApplyStatBuff(creature, haste, STAT_TYPE::HASTE);
		ApplyCreatureBuff(creature, haste, STAT_TYPE::HASTE);
	}

	virtual void OnCreatureHPCalculated(cube::Creature* creature, float* hp) override {
		ApplyStatBuff(creature, hp, STAT_TYPE::HEALTH);
		ApplyCreatureBuff(creature, hp, STAT_TYPE::HEALTH);
	}

	virtual void OnCreatureResistanceCalculated(cube::Creature* creature, float* resistance) override {
		ApplyStatBuff(creature, resistance, STAT_TYPE::RESISTANCE);
		ApplyCreatureBuff(creature, resistance, STAT_TYPE::RESISTANCE);
	}

	virtual void OnCreatureRegenerationCalculated(cube::Creature* creature, float* regeneration) override {
		ApplyStatBuff(creature, regeneration, STAT_TYPE::STAMINA);
	}

	virtual void OnCreatureManaGenerationCalculated(cube::Creature* creature, float* manaGeneration) override {
		ApplyStatBuff(creature, manaGeneration, STAT_TYPE::MANA);
	}
};

// Export of the mod created in this file, so that the modloader can see and use it.
EXPORT Mod* MakeMod() {
	return new Mod();
}