#define DF_Debug

#include "../incl/FireBot.h"
#include "../incl/CardBaseSMJ.h"
#include "../incl/Util.h"
#include "../incl/LOAD.h"

broker* (FireBot::Bro) = NULL;

void FireBot::PrepareForBattle(const api::MapInfo& mapInfo, const api::Deck& deck)
{
	MISS;
	//std::cout << "Prepare for: " << mapInfo.map << std::endl;
	//for(auto C: deck.cards)	MISD("CardID:" + std::to_string(C));
	//MySMJDeck(deck);
	SMJDeck.clear();
	for (auto apiCard : deck.cards)
		SMJDeck.push_back(Bro->J->CardFromJson(apiCard % 1000000));
	for (auto C : SMJDeck)
		MISD(C.cardName);
#ifdef MIS_Stream 
	Bro->L_GamesPlus();
#endif

	MISE;
}


std::vector<api::Deck> FireBot::DecksForMap(const api::MapInfo& mapInfo) 
{
	MISS;	
	
	//MISD("MAP: " + mapInfo.map);
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
	
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::Strikers, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::Mine, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::Spitfire, api::Upgrade_U3);

	v.push_back(deck);

	MISE;
	return v;
}

void FireBot::MatchStart(const api::GameStartState& state) 
{
	MISS;
	
	for (int iPlayerCount = 0; iPlayerCount < state.players.size(); iPlayerCount++)
	{
		
		if (state.players[iPlayerCount].entity.id == state.your_player_id)
		{
			myId = state.players[iPlayerCount].entity.id;
			imyPlayerIDX = iPlayerCount;
		}

		if (state.players[iPlayerCount].entity.id != state.your_player_id && state.players[iPlayerCount].entity.power > 0)
		{
			opId = state.players[iPlayerCount].entity.id;
			iopPlayerIDX = iPlayerCount;
		}
	}

	MISD("MY ID: " + std::to_string(myId));
	MISD("OP ID: " + std::to_string(opId));	
		
	MISE;
}

std::vector<api::Command> FireBot::Tick(const api::GameState & state) 
{		
	//MISS;
	MISD(std::to_string(iStage) +  "#" + Bro->sTime(state.current_tick) + "#" + std::to_string(state.current_tick) + "#" + std::to_string(state.players[imyPlayerIDX].power));
	auto v = std::vector<api::Command>();
	if (iSkipTick > 0)
	{
		MISD("SKIP");
		iSkipTick--;
		return v;
	}

	for (auto r : state.rejected_commands)
	{
		MISD("rejected Player: " + std::to_string(r.player));		
		MISD("reason         : " + Bro->U->switchCommandRejectionReason(r.reason));
		MISD("command        : " + Bro->U->switchCommand(r.command));
	}
	if (Bro->L->StartType == 1 && state.current_tick <= 100)
	{
		//std::vector<api::Entity> OPs = entitiesTOentity(opId, state.entities.squads[0]);
		if (state.entities.squads.size() == 0)return v;
		auto spawn = api::CommandProduceSquad();
		spawn.card_position = CardPicker(state.entities.squads[0].card_id, true);
		spawn.xy = api::to2D(entitiesTOentity(myId, state.entities.token_slots)[0].position);
		auto cmd = api::Command();
		cmd.v = spawn;
		v.push_back(cmd);

		iSkipTick = 40;
		Bro->L->StartType = 0;		
	}

	//WELL KILLER
	if (Bro->L->WellKiller)
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
	if (Bro->L->UnitEruption)
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
	
	//Take powerwells
	if (entitiesTOentity(myId,state.entities.power_slots).size() < 4 && state.current_tick % 5
		&& Bro->L->StartType == 0)
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
				spawn.xy = Bro->U->A_B_Offsetter(api::to2D(A.position), api::to2D(B.position), CastRange);				
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

int FireBot::GetSwiftCounterFor(Card OP, bool PerfectCounter, bool Swift)
{
	MISS;
	
	
	for (unsigned int i = 0; i < SMJDeck.size(); i++)
	{
		if (PerfectCounter == false && SMJDeck[i].offenseType == OP.defenseType
			||
			PerfectCounter == true && SMJDeck[i].offenseType == OP.defenseType && SMJDeck[i].defenseType != OP.offenseType)
		{
			if (!Swift)
			{
				MISEA("Not Swift: " + SMJDeck[i].cardName);
				return i;
			}

			for (Ability A : SMJDeck[i].abilities)
				if (A.abilityIdentifier == "Swift")
				{
					MISEA("Fast: " + SMJDeck[i].cardName);
					return i;
				}
		}				
	}
		
	MISEA("No Counter :-(");
	return -1;
}
int FireBot::CardPicker(api::CardId opID, bool Swift)
{
	MISS;
	Card OP = Bro->J->CardFromJson(opID % 1000000);

	int iReturn;
	iReturn = GetSwiftCounterFor(OP, true, Swift);
	if(iReturn == -1) iReturn = GetSwiftCounterFor(OP, false, Swift);
	if (iReturn == -1 && Swift)
	{
		iReturn = GetSwiftCounterFor(OP, true, false);
		if (iReturn == -1) iReturn = GetSwiftCounterFor(OP, false, false);
	}
	if (iReturn == -1)iReturn = 0;
	MISE;
	return iReturn;
}


void run_FireBot(broker* Bro, unsigned short port)
{
	MISS;
	FireBot::learnBro(Bro);
	auto bot = FireBot();		
	run(bot, port);
	MISE;
}

