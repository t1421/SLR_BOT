#define DF_Debug

#include "../incl/FireBot.h"
#include "../incl/Util.h"

broker* (FireBot::Bro) = NULL;

void FireBot::PrepareForBattle(const api::MapInfo& mapInfo, const api::Deck& deck){
	MISS;
	std::cout << "Prepare for: " << mapInfo.map << std::endl;
	oponents.clear();
	MISE;
}


std::vector<api::Deck> FireBot::DecksForMap(const api::MapInfo& mapInfo) {
	MISS;	
	
	MISD("MAP: " + mapInfo.map);
	//MISD("COM: " + mapInfo.community_map_details->name);
	unsigned int i = 0;
	auto v = std::vector<api::Deck>();
	auto deck = api::Deck();
	deck.name = "FireDeck";
	deck.cover_card_index = 19;
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::Scavenger, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::Sunstriders, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::Thugs, api::Upgrade_U3);	
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::Wrecker, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::Sunderer, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::FireswornAFire, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::Eruption, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::ScorchedEarthAFire, api::Upgrade_U3);

	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::Enforcer, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::FiredancerPromo, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::ScytheFiends, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::SkyfireDrake, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::GladiatrixANature, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::LavaField, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::Wildfire, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::Ravage, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::RallyingBanner, api::Upgrade_U3);
	
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::JuggernautPromo, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::GiantSlayer, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::Spitfire, api::Upgrade_U3);

	v.push_back(deck);

	MISE;
	return v;
}

void FireBot::MatchStart(const api::GameStartState& state) {
	MISS;
	
	for (auto p : state.players)
	{
		if (p.entity.id != state.your_player_id) opId = p.entity.id;
	}
	myId = state.your_player_id;
	MISD("MY ID: " + std::to_string(myId));
	MISD("OP ID: " + std::to_string(opId));
		
	MISE;
}

//std::vector<api::APICommand> FireBot::Tick(const api::APIGameState& state) {
std::vector<api::Command> FireBot::Tick(const api::GameState & state) {

	//std::vector<std::vector<api::Entity>> allEntities;
	//std::vector<api::Entity> allEntities;
	//state.entities.squads[0].entity.position.x;
	
	//MISS;
	MISD(std::to_string(iStage) +  "#" + Bro->sTime(state.current_tick) + "#" + std::to_string(state.current_tick) + "#" + std::to_string(state.players[imyPlayerIDX].power));
	//std::cout << "tick " << state.current_tick << " entities count: " << state.entities.size() << std::endl;
	auto v = std::vector<api::Command>();
	

	for (auto r : state.rejected_commands)
	{
		MISD("rejected Player: " + std::to_string(r.player));		
		MISD("reason         : " + Bro->U->switchCommandRejectionReason(r.reason));
		MISD("command        : " + Bro->U->switchCommand(r.command));
	}

	if (once)
	{
		if (state.players.size() - 1 < iPlayerCount)
			for (iPlayerCount = 0; iPlayerCount < state.players.size(); iPlayerCount++)
				if (state.players[iPlayerCount].id == myId)
					imyPlayerIDX = iPlayerCount;
		/*
		MISD("Buildings");
		for (auto e : state.entities.buildings)
		{
			MISD(std::to_string(e.entity.id ) + "#" +
				//std::to_string(e.player_entity_id.value()) + "#" +
				std::to_string(e.entity.position.x) + "_" +
				std::to_string(e.entity.position.y) + "_" +
				std::to_string(e.entity.position.z) + "#" +
				Bro->B->switchAPIEntitySpecific(e.entity.specific));
		}
		*/
		MISD("Wells");
		for (auto e : state.entities.power_slots)
		{
			MISD(std::to_string(e.entity.id) + "#" +
				std::to_string(e.entity.player_entity_id.has_value()) + "#" +
				std::to_string(e.entity.position.x) + "_" +
				std::to_string(e.entity.position.y) + "_" +
				std::to_string(e.entity.position.z) + "#" );
		}
		/*
		MISD("FreeOrbs");
		for (auto e : FreeOrbs)
		{
			MISD(std::to_string(e.id) + "#" +
				//std::to_string(e.player_entity_id.value()) + "#" +
				std::to_string(e.position.x) + "_" +
				std::to_string(e.position.y) + "_" +
				std::to_string(e.position.z) + "#" +
				Bro->B->switchAPIEntitySpecific(e.specific));
		}
		*/
		once = false;
	}

	

	//WELL KILLER
	if (true)
	{
		for (auto W : state.entities.power_slots)
		{
			if (W.entity.player_entity_id != opId)continue;

			for (auto A : W.entity.aspects)
			{
				if (std::get_if<api::AspectHealth>(&A.v))
				{
					if (std::get<api::AspectHealth>(A.v).current_hp <= 300)
					{
						MISD("FIRE !!!!");
						auto spell = api::CommandCastSpellGod();
						spell.card_position = 6;
						spell.target.v = api::SingleTargetLocation(api::to2D(W.entity.position));
						auto cmd = api::Command();
						cmd.v = spell;
						v.push_back(cmd);
					}
				}							
			}
		}
	}

	//Cool eruption
	if (true)
	{
		std::vector<api::Entity> vTemp;
		for (auto U : state.entities.squads)
		{
			if (U.entity.player_entity_id != opId)continue;

			//vTemp = Bro->U->pointsInRadius(Bro->U->entitiesTOentity(state.entities.squads), api::to2D(U.entity.position), 10);
			vTemp = Bro->U->pointsInRadius(entitiesTOentity(opId,state.entities.squads), api::to2D(U.entity.position), 10);
			if (vTemp.size() >=3)
			{
				MISD("FIRE !!!!");
				auto spell = api::CommandCastSpellGod();
				spell.card_position = 6;
				spell.target.v = api::SingleTargetLocation(api::to2D(U.entity.position));
				auto cmd = api::Command();
				cmd.v = spell;
				v.push_back(cmd);
			}			
		}
	}
	
	if (entitiesTOentity(myId,state.entities.power_slots).size() < 4 && state.current_tick % 5)
	{
		api::Entity A;
		api::Entity B;
		float fDistanc = 0;
		
		if (entitiesTOentity(myId, state.entities.squads).size() == 0)
		{
			fDistanc = Bro->U->CloseCombi(entitiesTOentity(myId, state.entities.squads, state.entities.buildings, state.entities.power_slots, state.entities.token_slots),
				entitiesTOentity(0, state.entities.power_slots), A, B);

			if (state.players[imyPlayerIDX].power > 75)
			{
				auto spawn = api::CommandProduceSquad();
				spawn.card_position = 0; // Code für fast unit
				//spawn.xy = api::to2D(MyBuildings[iHelperA].position);				
				spawn.xy = Bro->U->A_B_Offsetter(api::to2D(A.position), api::to2D(B.position), CastRange);
				//MISD("My Building: " + std::to_string(MyBuildings[iHelperA].position.x) + "#" + std::to_string(MyBuildings[iHelperA].position.z));
				//MISD("My Traget  : " + std::to_string(FreeWells[iHelperB].position.x) + "#" + std::to_string(FreeWells[iHelperB].position.z));
				//MISD("SPawn at   : " + std::to_string(spawn.xy.x) + "#" + std::to_string(spawn.xy.y));
				//MISD("Distanc    : " + std::to_string(CastRange));
				auto cmd = api::Command();
				cmd.v = spawn;
				v.push_back(cmd);
			}
		}
		else //Move Unit
		{
			fDistanc = Bro->U->CloseCombi(entitiesTOentity(myId, state.entities.squads), 
				entitiesTOentity(0, state.entities.power_slots), A, B);
			auto move = api::CommandGroupGoto();
			move.squads = { A.id};
			move.positions = { api::to2D(B.position) };
			move.walk_mode = api::WalkMode_Normal;
			auto cmd = api::Command();
			cmd.v = move;
			v.push_back(cmd);
		}

		if (fDistanc < CastRange && state.players[imyPlayerIDX].power >= 100)
		{
			auto build = api::CommandPowerSlotBuild();
			build.slot_id = B.id;
			auto cmd = api::Command();
			cmd.v = build;
			v.push_back(cmd);
			iStage++;
		}
	}
	
	//MISE;
	return v;
}



void run_FireBot(broker* Bro, unsigned short port)
{
	MISS;
	FireBot::learnBro(Bro);
	auto bot = FireBot();		
	run(bot, port);
	MISE;
}