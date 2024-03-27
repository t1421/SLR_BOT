﻿#define DF_Debug

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
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::FireswornAFrost, api::Upgrade_U3);
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
	
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::BurningSpears, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::DisenchantANature, api::Upgrade_U3);
	
	//deck.cards[i++] = api::CardIdWithUpgrade(card_templates::Strikers, api::Upgrade_U3);
	//deck.cards[i++] = api::CardIdWithUpgrade(card_templates::Mine, api::Upgrade_U3);
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
	CoolEruptionTest.s = true;
		
	MISE;
}

std::vector<api::Command> FireBot::Tick(const api::GameState & state) 
{		
	//MISS;
	MISD(std::to_string(eStage) +  "#" + Bro->sTime(state.current_tick) + "#" + std::to_string(state.current_tick) + "#" + std::to_string(state.players[imyPlayerIDX].power));
	auto v = std::vector<api::Command>();
	if (iSkipTick > 0)
	{
		MISD("SKIP");
		iSkipTick--;
		return v;
	}

	if (state.current_tick % 100)Stage(state);

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
		v.push_back(api::Command(spawn));

		iSkipTick = 40;
		Bro->L->StartType = 0;		
	}

	//WELL KILLER
	if (Bro->L->WellKiller)WellKiller(v, entitiesTOentity(opId, state.entities.power_slots));
	if (Bro->L->AvoidArea)
	{
		//Find New Stuff
		FindAvoidArea(state);

		//Remove Old Stuff
		if(vAvoid.size() > 0)RemoveFromMIS_AvoidArea(state.current_tick);

		//Avoid Stuff
		if (vAvoid.size() > 0)for (auto vv : MoveUnitsAway(state))v.push_back(vv);
	}

	//Cool eruption
	if (Bro->L->UnitEruption && CoolEruptionTest.s)
	{
		CoolEruptionTest.f = std::async(&FireBot::CoolEruption, this, state);
		CoolEruptionTest.s = false; // dont start again
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
				v.push_back(api::Command(spawn));
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
			v.push_back(api::Command(move));
		}

		if (fDistanc < CastRange && state.players[imyPlayerIDX].power >= 100)
		{
			auto build = api::CommandPowerSlotBuild();
			build.slot_id = B.id;
			v.push_back(api::Command(build));
			//iStage++;
		}
	}

	//Spam and run to Base
	if (state.current_tick % 5 && Bro->L->StartType == 2)
	{
		api::Entity A;
		api::Entity B;
		float fDistanc = 0;


		fDistanc = Bro->U->CloseCombi(entitiesTOentity(myId, state.entities.squads, state.entities.buildings, state.entities.power_slots, state.entities.token_slots),
			entitiesTOentity(opId, state.entities.power_slots, state.entities.token_slots), A, B);

		if (state.players[imyPlayerIDX].power > 75)
		{
			auto spawn = api::CommandProduceSquad();
			spawn.card_position = 0; // Code für fast unit
			spawn.xy = Bro->U->A_B_Offsetter(api::to2D(A.position), api::to2D(B.position), CastRange);
			v.push_back(api::Command(spawn));
		}

		for (auto E : entitiesTOentity(myId, state.entities.squads))
		{
			//MISD(Bro->U->switchCommandJob(E.job))

			if (std::get_if<api::JobIdle>(&E.job.v))
			{
				auto move = api::CommandGroupGoto();
				move.squads = { E.id };
				move.positions = { api::to2D(B.position) };
				move.walk_mode = api::WalkMode_Normal;
				v.push_back(api::Command(move));
			}
			
		}

	}

	

	if (CoolEruptionTest.f.wait_for(0ms) == std::future_status::ready)
	{
		for (auto vv : CoolEruptionTest.f.get())v.push_back(vv);
		CoolEruptionTest.s = true;
	}

	//MISE;
	return v;
}

std::vector<api::Command> FireBot::CoolEruption(const api::GameState& state)
{
	MISS;

	auto vReturn = std::vector<api::Command>();

	std::vector<api::Entity> vTemp;
	for (auto U : state.entities.squads)
	{
		if (U.entity.player_entity_id != opId)continue;

		//vTemp = Bro->U->pointsInRadius(Bro->U->entitiesTOentity(state.entities.squads), api::to2D(U.entity.position), 10);
		vTemp = Bro->U->pointsInRadius(entitiesTOentity(opId, state.entities.squads), api::to2D(U.entity.position), 10);
		if (vTemp.size() >= 3)
		{
			MISD("FIRE !!!!");
			auto spell = api::CommandCastSpellGod();
			spell.card_position = 6;
			spell.target.v = api::SingleTargetLocation(api::to2D(U.entity.position));
			auto cmd = api::Command();
			cmd.v = spell;
			vReturn.push_back(cmd);
			break; //only for one unit not all 3
		}
	}

	MISE;
	return vReturn;	
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

void FireBot::WellKiller(std::vector<api::Command> vMain, std::vector<api::Entity> Wells)
{
	MISS;	
	for (auto W : Wells)
	{
		for (auto A : W.aspects)
		{
			if (std::get_if<api::AspectHealth>(&A.v))
			{
				if (std::get<api::AspectHealth>(A.v).current_hp <= 300)
				{
					MISD("FIRE !!!!");
					auto spell = api::CommandCastSpellGod();
					spell.card_position = 6;
					// Spot on, maye chaneg to offset so i can hit units
					spell.target.v = api::SingleTargetLocation(api::to2D(W.position)); 
					vMain.push_back(api::Command(spell));
				}
			}
		}
	}
	MISE;
}

void FireBot::FindAvoidArea(const api::GameState& state)
{
	MISS;

	for (auto O : state.entities.ability_world_objects)
	{
		//Only OP objects and only New ons
		if (   O.entity.player_entity_id.value() != opId
			|| O.entity.id <= MaxAvoidID)continue;
		
		MaxAvoidID = O.entity.id;

		for (auto E : O.entity.effects)
		{

			/*
			if (E.id % 1000000 == 290) //Cold snap
				MISD("Snap at: " + std::to_string(O.entity.position.x) + "#" + std::to_string(O.entity.position.x));			
			
			if (E.id % 1000000 == 1006) //Creeping Paralyse
			{
				MISD("Creeping at: " + std::to_string(O.entity.position.x) + "#" + std::to_string(O.entity.position.x));
				//MISD(E.start_tick);
				printf("%i -> %i\n", E.start_tick, E.end_tick);					
			}

			if (E.id % 1000000 == 1776) //Huricane
			{
				MISD("Huricane at: " + std::to_string(O.entity.position.x) + "#" + std::to_string(O.entity.position.x));
				printf("%i -> %i\n", E.start_tick, E.end_tick);
			}*/

			if (E.id % 1000000 == 1620) 
			{
				MISD("Add Glyth til " + std::to_string(E.end_tick.value()));
				MISD(std::get<api::AbilityEffectSpecificSpellOnEntityNearby>(E.specific.v).radius);
				vAvoid.push_back(new MIS_AvoidArea(
					E.end_tick.value(),
					O.entity.position,
					std::get<api::AbilityEffectSpecificSpellOnEntityNearby>(E.specific.v).radius));
			}
			
			//std::get<api::AreaShapeCircle>(std::get<api::AbilityEffectSpecificDamageArea>(E.specific.v).shape.v).				
			// Area Spells
			//  1620 // Glyph of Frost
			//   290 // Cold Snap
			//  1471 // Aura Of Corruption
			//  1694 // Ensnaring Roots
			//  1006 // Creeping Paralyse

			//std::get<api::AreaShapeWideLine>(std::get<api::AbilityEffectSpecificDamageArea>(E.specific.v).shape.v).
			// Line Spells
			// 11003 // Bandit Minefield
			//  1776 // Huricane
			//  1740 // Wildfire
			
		}
	}

	MISE;
}

bool FireBot::Stage(const api::GameState& state)
{
	MISS;

	

	//if units close to my Well / Orb -> Def.

	//if nothing to do - maybe Save Power

	//If time is right Well / Orb

	//if Def is done -> Atack
	//Or when power is saved
	//or when OP build well

	MISE;
	return true;
}


void FireBot::RemoveFromMIS_AvoidArea(api::Tick curTick)
{
	MISS;
	for (std::vector<MIS_AvoidArea*>::iterator it = vAvoid.begin(); it != vAvoid.end();)
	{
		if ((*it)->endTick <= curTick)
		{
			MISD("Remove Data")
			it = vAvoid.erase(it);
		}
		else  ++it;
	}
	MISE;
}


std::vector<api::Command> FireBot::MoveUnitsAway(const api::GameState& state)
{
	MISS;

	auto vReturn = std::vector<api::Command>();

	for(auto A: vAvoid)
	{
		for (auto EE : Bro->U->pointsInRadius(entitiesTOentity(myId, state.entities.squads), api::to2D(A->pos), A->radius * 1.1))
		{
			MISD("MOVE Units");
			auto move = api::CommandGroupGoto();
			move.squads = { EE.id };
			move.positions = { (Bro->U->A_B_Offsetter(api::to2D(EE.position), api::to2D(A->pos), A->radius * -1 * 1.2)) };
			move.walk_mode = api::WalkMode_Normal;
			vReturn.push_back(api::Command(move));
		}
	}

	MISE;
	return vReturn;
}
