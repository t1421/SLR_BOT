#include "../incl/DumpBot.h"

#include <fstream>

broker* (DumpBot::Bro) = NULL;


bool run_DumpBot(broker* Bro, unsigned short port, std::string sName)
{
	MISS;
	DumpBot::learnBro(Bro);
	auto bot = DumpBot(sName);
	run(bot, port);
	MISE;
	return true;
}

void DumpBot::PrepareForBattle(const capi::MapInfo& mapInfo, const capi::Deck& deck)
{
	MISS;
	MISD("PrepareForBattle: " + std::to_string(mapInfo.map) + " with " + deck.name);
	MISE;
}

std::vector<capi::Deck> DumpBot::DecksForMap(const capi::MapInfo& mapInfo)
{
	MISS;	
	
	MISD("DecksForMap: " + mapInfo.map);
	MISD("Name: " + Name);
	unsigned int i = 0;
	auto v = std::vector<capi::Deck>();
	auto deck = capi::Deck();

		deck.name = "FireDeck";
		i = 0;

		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Scavenger, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Sunstriders, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Thugs, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Wrecker, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Sunderer, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::FireswornAFrost, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Eruption, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::ScorchedEarthAFire, capi::Upgrade_U3);

		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Enforcer, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Firedancer, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::ScytheFiends, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::SkyfireDrake, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::GladiatrixANature, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::LavaField, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Wildfire, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Ravage, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::RallyingBanner, capi::Upgrade_U3);

		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::BurningSpears, capi::Upgrade_U3);
		//deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::DisenchantANature, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::GiantSlayer, capi::Upgrade_U3);

		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Juggernaut, capi::Upgrade_U3);
		deck.cover_card_index = 19;

		v.push_back(deck);
	
	MISE;
	return v;
}

void DumpBot::MatchStart(const capi::GameStartState& state)
{
	MISS;
	
	MISERROR(("MatchStart at " + std::to_string(Bro->L_getEEE_Now())).c_str());
	
	MISE;
}

capi::TickResponse DumpBot::Tick(const capi::GameState& state)
{		
	auto v = std::vector<capi::Command>();
	auto vX = capi::TickResponse{};

	auto effect_json = nlohmann::json(state).dump();
	std::ofstream file("DUMP_at_" + std::to_string(state.current_tick) + ".json");
	file << effect_json << std::endl;
	file.close();

	vX.commands = v;
	return vX;
}

