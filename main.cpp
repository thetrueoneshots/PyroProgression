#include "main.h"

// Public variables.
// For example: int number = 10;

// Includes for the self written hooks.
// For example: #include "src/hooks/a_hook.h" 
#include "src/XPOverwrite.h"

/* Mod class containing all the functions for the mod.
*/
class Mod : GenericMod {

	std::vector<cube::TextFX> fx_list;
	/* Hook for the chat function. Triggers when a user sends something in the chat.
	 * @param	{std::wstring*} message
	 * @return	{int}
	*/
	virtual int OnChat(std::wstring* message) override {
		return 0;
	}

	virtual void OnGameUpdate(cube::Game* game) override {
		if (fx_list.size() > 0)
		{
			game->textfx_list.push_back(fx_list.at(0));
			fx_list.erase(fx_list.begin());
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

			game->PlaySoundEffect(cube::Game::SoundEffect::sound_level_up);

			FloatRGBA purple(0.65f, 0.40f, 1.0f, 1.0f);

			cube::TextFX xpText = cube::TextFX();
			xpText.position = player->entity_data.position;
			xpText.animation_length = 3000;
			xpText.distance_to_fall = -500;
			xpText.color = purple;
			xpText.size = 64;
			xpText.offset_2d = FloatVector2(0, 0);
			xpText.text = L"LEVEL UP!\n";
			xpText.field_60 = 0;

			game->textfx_list.push_back(xpText);
			game->PrintMessage(L"LEVEL UP!\n", &purple);
		}

		return;
	}

	virtual void OnCreatureDeath(cube::Game* game, cube::Creature* creature, cube::Creature* attacker) override {
		if (attacker == nullptr || creature == nullptr)
		{
			return;
		}

		cube::Creature* player = game->GetPlayer();
		if (player->id == attacker->id || player->pet_id == attacker->id)
		{
			FloatRGBA purple(0.65f, 0.40f, 1.0f, 1.0f);
			int xp_gain = creature->entity_data.level + 1;

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
				fx_list.push_back(xpText);
			}

			player->entity_data.XP += xp_gain;
		}
	}

	/* Function hook that gets called on intialization of cubeworld.
	 * [Note]:	cube::GetGame() is not yet available here!!
	 * @return	{void}
	*/
	virtual void Initialize() override {
		Setup_XP_Overwrite();
		return;
	}
};

// Export of the mod created in this file, so that the modloader can see and use it.
EXPORT Mod* MakeMod() {
	return new Mod();
}