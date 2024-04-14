#define DF_Debug

#include "../incl/FireBot.h"
#include "../incl/CardBaseSMJ.h"
#include "../incl/Util.h"
#include "../incl/LOAD.h"

broker* (FireBot::Bro) = NULL;

void FireBot::PrepareForBattle(const capi::MapInfo& mapInfo, const capi::Deck& deck)
{
	MISS;
	MISD("PrepareForBattle: " + std::to_string(mapInfo.map) + " with " + deck.name);
	vAvoid.clear();
	MaxAvoidID = 0;

	SMJDeck.clear();
	for (auto apiCard : deck.cards)	SMJDeck.push_back(Bro->J->CardFromJson(apiCard % 1000000));
#ifdef MIS_Stream 
	Bro->L_GamesPlus();
#endif

	MISE;
}

std::vector<capi::Deck> FireBot::DecksForMap(const capi::MapInfo& mapInfo)
{
	MISS;	
	
	MISD("DecksForMap: " + mapInfo.map)
	unsigned int i = 0;
	auto v = std::vector<capi::Deck>();
	auto deck = capi::Deck();
	deck.name = "FireDeck";
	deck.cover_card_index = 19;
	deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Scavenger, capi::Upgrade_U3);
	deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Sunstriders, capi::Upgrade_U3);
	deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Thugs, capi::Upgrade_U3);	
	deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Wrecker, capi::Upgrade_U3);
	deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Sunderer, capi::Upgrade_U3);
	deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::FireswornAFrost, capi::Upgrade_U3);
	deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Eruption, capi::Upgrade_U3);
	deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::ScorchedEarthAFire, capi::Upgrade_U3);

	deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Enforcer, capi::Upgrade_U3);
	deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::FiredancerPromo, capi::Upgrade_U3);
	deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::ScytheFiends, capi::Upgrade_U3);
	deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::SkyfireDrake, capi::Upgrade_U3);
	deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::GladiatrixANature, capi::Upgrade_U3);
	deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::LavaField, capi::Upgrade_U3);
	deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Wildfire, capi::Upgrade_U3);
	deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Ravage, capi::Upgrade_U3);
	deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::RallyingBanner, capi::Upgrade_U3);
	
	deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::BurningSpears, capi::Upgrade_U3);
	deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::DisenchantANature, capi::Upgrade_U3);
	
	deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Spitfire, capi::Upgrade_U3);

	v.push_back(deck);

	MISE;
	return v;
}

void FireBot::MatchStart(const capi::GameStartState& state)
{
	MISS;

	MISD("MatchStart");
	
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
	GlobalBattleTable.s = true;
		
	MISE;
}


std::vector<capi::Command> FireBot::Tick(const capi::GameState& state)
{		
#ifdef MIS_DEBUG

	if(Bro->L->AllTick)	MISD(std::to_string(eStage) + "#" + Bro->sTime(state.current_tick) + "#" + std::to_string(state.current_tick) + "#" + std::to_string(state.players[imyPlayerIDX].power));

	if (Bro->sComand != "")
	{
		if (Bro->sComand == "?")
		{
			MISERROR("------------------------------");
			MISERROR("# dump     = Save tick        ");
			MISERROR("# bt       = Print BattleTable");
			MISERROR("------------------------------");
		}

		if (Bro->sComand == "dump")
		{
			auto effect_json = nlohmann::json(state).dump();
			std::ofstream file("DUMP_at_" + std::to_string(state.current_tick) + ".json");
			file << effect_json << std::endl;
			file.close();
			Bro->sComand = "";
		}

		if (Bro->sComand == "bt")
		{
			EchoBattleTable(myBT);
			EchoBattleTable(opBT);
		}
		Bro->sComand = "";
	}
#endif

	auto v = std::vector<capi::Command>();
	
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
		if (state.entities.squads.size() == 0)return v;
		auto spawn = capi::CommandProduceSquad();
		spawn.card_position = CardPicker(state.entities.squads[0].card_id, true);
		spawn.xy = capi::to2D(entitiesTOentity(myId, state.entities.token_slots)[0].position);
		v.push_back(capi::Command(spawn));

		iSkipTick = 40;
		Bro->L->StartType = 0;		
	}
	
	///////////////Timing critical ////////////////
	//WELL KILLER
	if (Bro->L->WellKiller)
		if(WellKiller(v, entitiesTOentity(opId, state.entities.power_slots)))
			return v;
	
	//Avoid Spells
	if (Bro->L->AvoidArea)
	{
		//Find New Stuff
		FindAvoidArea(state);

		//Remove Old Stuff
		if(vAvoid.size() > 0)RemoveFromMIS_AvoidArea(state.current_tick);

		//Avoid Stuff
		if (vAvoid.size() > 0)for (auto vv : MoveUnitsAway(state))v.push_back(vv);

#ifdef MIS_DEBUG
		if (state.current_tick % 50 == 0 && Bro->L->DrawAvoidArea)
			for (auto vv : vAvoid)							
				for (auto vvv : Bro->U->DrawCircle(vv->pos, vv->radius))
					v.push_back(vvv);
#endif // MIS_DEBUG			
						
	}

	/////////////THREADS////////////////

	//Cool eruption
	if (Bro->L->UnitEruption && CoolEruptionTest.s)
	{
		CoolEruptionTest.fc = std::async(&FireBot::CoolEruption, this, state);
		CoolEruptionTest.s = false; // dont start again
	}
	
	if (Bro->L->BattleTable && GlobalBattleTable.s && state.current_tick % 50)
	{
		GlobalBattleTable.fb = std::async(&FireBot::CalGlobalBattleTable, this, state);
		GlobalBattleTable.s = false; // dont start again
	}
	

	////////////////Normal Stuff ////////////////////

	//Take powerwells
	if (entitiesTOentity(myId,state.entities.power_slots).size() < 4 && state.current_tick % 5
		&& Bro->L->StartType == 0)
	{
		capi::Entity A;
		capi::Entity B;
		float fDistanc = 0;
		
		if (entitiesTOentity(myId, state.entities.squads).size() == 0)
		{			
			fDistanc = Bro->U->CloseCombi(entitiesTOentity(myId, state.entities.squads, state.entities.buildings, state.entities.power_slots, state.entities.token_slots),
				entitiesTOentity(0, state.entities.power_slots), A, B);

			if (state.players[imyPlayerIDX].power > 75)
			{
				auto spawn = capi::CommandProduceSquad();
				spawn.card_position = 0; // Code für fast unit
				spawn.xy = Bro->U->Offseter(capi::to2D(A.position), capi::to2D(B.position), CastRange);
				v.push_back(capi::Command(spawn));
			}
		}
		else //Move Unit
		{
			fDistanc = Bro->U->CloseCombi(entitiesTOentity(myId, state.entities.squads), 
				entitiesTOentity(0, state.entities.power_slots), A, B);
			auto move = capi::CommandGroupGoto();
			move.squads = { A.id};
			move.positions = { capi::to2D(B.position) };
			move.walk_mode = capi::WalkMode_Normal;
			v.push_back(capi::Command(move));
		}
		if (fDistanc < CastRange && state.players[imyPlayerIDX].power >= 100)
		{
			auto build = capi::CommandPowerSlotBuild();
			build.slot_id = B.id;
			v.push_back(capi::Command(build));
			//iStage++;
		}
	}

	//Spam and run to Base
	if (state.current_tick % 5 == 0 && Bro->L->StartType == 2)
	{
		capi::Entity A;
		capi::Entity B;
		float fDistanc = 0;


		fDistanc = Bro->U->CloseCombi(entitiesTOentity(myId, state.entities.squads, state.entities.buildings, state.entities.power_slots, state.entities.token_slots),
			entitiesTOentity(opId, state.entities.power_slots, state.entities.token_slots), A, B);

		if (state.players[imyPlayerIDX].power > 75)
		{
			auto spawn = capi::CommandProduceSquad();
			spawn.card_position = 0; // Code für fast unit
			spawn.xy = Bro->U->Offseter(capi::to2D(A.position), capi::to2D(B.position), CastRange);
			v.push_back(capi::Command(spawn));
		}

		for (auto E : entitiesTOentity(myId, state.entities.squads))
		{
			if (E.job.variant_case == capi::JobCase::NoJob)
			{
				auto move = capi::CommandGroupGoto();
				move.squads = { E.id };
				move.positions = { capi::to2D(B.position) };
				move.walk_mode = capi::WalkMode_Normal;
				v.push_back(capi::Command(move));
			}
			
		}

	}

	if(Bro->L->UnitEruption && CoolEruptionTest.s == false)
		if (CoolEruptionTest.fc.wait_for(0ms) == std::future_status::ready)
		{
			for (auto vv : CoolEruptionTest.fc.get())v.push_back(vv);
			CoolEruptionTest.s = true;
		}
	
	if (Bro->L->BattleTable && GlobalBattleTable.s == false)
		if (GlobalBattleTable.fb.wait_for(0ms) == std::future_status::ready)
			GlobalBattleTable.s = true;
	

	//Remove dobbel orders
	//Finde idel units
	//dont cast unit int avoid area

	//Iff rooted skip order to move out of avoid

	return v;
}

std::vector<capi::Command> FireBot::CoolEruption(const capi::GameState& state)
{
	MISS;

	auto vReturn = std::vector<capi::Command>();

	std::vector<capi::Entity> vTemp;
	for (auto U : state.entities.squads)
	{
		if (U.entity.player_entity_id != opId)continue;

		//vTemp = Bro->U->pointsInRadius(Bro->U->entitiesTOentity(state.entities.squads), capi::to2D(U.entity.position), 10);
		vTemp = Bro->U->pointsInRadius(entitiesTOentity(opId, state.entities.squads), capi::to2D(U.entity.position), 10);
		if (vTemp.size() >= 3)
		{
			MISD("FIRE !!!!");
			auto spell = capi::CommandCastSpellGod();
			spell.card_position = 6;
			spell.target = capi::SingleTargetLocation(capi::to2D(U.entity.position));			
			vReturn.push_back(capi::Command(spell));
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
int FireBot::CardPicker(capi::CardId opID, bool Swift)
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


bool run_FireBot(broker* Bro, unsigned short port)
{
	MISS;
	FireBot::learnBro(Bro);
	auto bot = FireBot();		
	run(bot, port);
	MISE;
	return true;
}

bool FireBot::WellKiller(std::vector<capi::Command> vMain, std::vector<capi::Entity> Wells)
{
	MISS;	
	for (auto W : Wells)
	{
		for (auto A : W.aspects)
		{
			//if (std::get_if<capi::AspectHealth>(&A.v))
			if (A.variant_case==capi::AspectCase::Health)
			{
				if (A.variant_union.health.current_hp <= 300)
				{
					MISD("FIRE !!!!");
					auto spell = capi::CommandCastSpellGod();
					spell.card_position = 6;
					// Spot on, maye chaneg to offset so i can hit units
					spell.target = capi::SingleTargetLocation(capi::to2D(W.position));
					vMain.push_back(capi::Command(spell));
					return true;
				}
			}
		}
	}
	MISE;
	return false;
}

void FireBot::FindAvoidArea(const capi::GameState& state)
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
			if (E.id % 1000000 == 1620) 
			{
				MISD("Add Glyth til " + std::to_string(E.end_tick.value()));
				vAvoid.push_back(new MIS_AvoidArea(
					E.end_tick.value(),
					capi::to2D(O.entity.position),
					E.specific.variant_union.spell_on_entity_nearby.radius));
			}
			if (E.id % 1000000 == 1694)
			{
				MISD("Add Root til " + std::to_string(E.end_tick.value()));
				vAvoid.push_back(new MIS_AvoidArea(
					E.end_tick.value(),
					capi::to2D(O.entity.position),
					E.specific.variant_union.aura.radius));					
			}
			if (E.id % 1000000 == 290)
			{
				MISD("Add Could snap til " + std::to_string(E.end_tick.value()));
				vAvoid.push_back(new MIS_AvoidArea(
					E.end_tick.value(),
					capi::to2D(O.entity.position),
					20));
			}
			if (E.id % 1000000 == 1471 || 
				E.id % 1000000 == 4427)
			{
				MISD("Add Aura of Coruption til " + std::to_string(E.end_tick.value()));
				vAvoid.push_back(new MIS_AvoidArea(
					E.end_tick.value(),
					capi::to2D(O.entity.position),
					30));
			}
			if (E.id % 1000000 == 1006)
			{
				MISD("Add Paralyse til " + std::to_string(E.end_tick.value()));
				vAvoid.push_back(new MIS_AvoidArea(
					E.end_tick.value(),
					capi::to2D(O.entity.position),
					20));
			}

			if (E.id % 1000000 == 11003)
			{
				MISD("Add Bandid Minfield til " + std::to_string(E.end_tick.value()));

				capi::Position2D EndPos;
				EndPos.x = capi::to2D(O.entity.position).x +
					E.specific.variant_union.moving_interval_cast.direction_step.x * 100;
				EndPos.y = capi::to2D(O.entity.position).y +
					E.specific.variant_union.moving_interval_cast.direction_step.y * 100;

				for (unsigned int i = 0; i < 4; i++)
				{
					vAvoid.push_back(new MIS_AvoidArea(
						E.end_tick.value(),
						Bro->U->Offseter(capi::to2D(O.entity.position),
							EndPos, 6 * i),
						10));
				}
				
			}
			if (E.id % 1000000 == 1740)
			{
				MISD("Add Wild Fire til " + std::to_string(E.end_tick.value()));

				capi::Position2D EndPos;
				EndPos.x = capi::to2D(O.entity.position).x +
					E.specific.variant_union.moving_interval_cast.direction_step.x * 100;
				EndPos.y = capi::to2D(O.entity.position).y +
					E.specific.variant_union.moving_interval_cast.direction_step.y * 100;

				int iRange = abs(Bro->U->distance(capi::to2D(O.entity.position), 
					EndPos));
				if (iRange <= 15)iRange = 15;

				for (unsigned int i = 0; i < 4; i++)
				{
					for (int j = -1; j < 2; j++)
					{
						vAvoid.push_back(new MIS_AvoidArea( //Tick1 L
							E.end_tick.value(),
							Bro->U->Offseter(
								capi::to2D(O.entity.position),
								EndPos, 
								iRange / 4.0f * i, 
								5.0f * j),
							5));						
					}
				}				
			}


			if (E.id % 1000000 == 1775)
			{
				MISD("Add Hurrican " + std::to_string(E.end_tick.value()));				

				for (unsigned int i = 0; i < 8; i++)
				{
					vAvoid.push_back(new MIS_AvoidArea( 
						E.end_tick.value(),
						Bro->U->Offseter(E.specific.variant_union.damage_area.shape.variant_union.wide_line.start,
							E.specific.variant_union.damage_area.shape.variant_union.wide_line.end,
						    50 / 8 * i),
						5));
				}
				
			}

		}
	}

	MISE;
}

bool FireBot::Stage(const capi::GameState& state)
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


void FireBot::RemoveFromMIS_AvoidArea(capi::Tick curTick)
{
	MISS;
	for (std::vector<MIS_AvoidArea*>::iterator it = vAvoid.begin(); it != vAvoid.end();)
	{
		if ((*it)->endTick <= curTick)
		{
			it = vAvoid.erase(it);
		}
		else  ++it;
	}
	MISE;
}


std::vector<capi::Command> FireBot::MoveUnitsAway(const capi::GameState& state)
{
	MISS;

	auto vReturn = std::vector<capi::Command>();

	for(auto A: vAvoid)
	{
		for (auto EE : Bro->U->pointsInRadius(entitiesTOentity(myId, state.entities.squads), A->pos, A->radius * 1.1))
		{
			MISD("MOVE Units");
			auto move = capi::CommandGroupGoto();
			move.squads = { EE.id };
			move.positions = { (Bro->U->Offseter(capi::to2D(EE.position), A->pos, A->radius * -1 * 1.2)) };
			move.walk_mode = capi::WalkMode_Normal;
			vReturn.push_back(capi::Command(move));
		}
	}

	MISE;
	return vReturn;
}


BattleTable FireBot::CalcBattleTable(std::vector<capi::Squad> squads)
{
	MISS;
	BattleTable BTreturn;
	BTreturn.Init();
	Card TempCard;
	for (auto S : squads)
	{
		TempCard = Bro->J->CardFromJson(S.card_id % 1000000);
		if (TempCard.offenseType < 0)TempCard.offenseType = 4; //fall back;
		if (TempCard.defenseType < 0)TempCard.defenseType = 4; //fall back;
		BTreturn.SizeCounter[TempCard.defenseType][TempCard.offenseType] += TempCard.health;

		if (TempCard.movementType == 1)BTreturn.Flyer++;
	}
	MISE;
	return BTreturn;
}

bool FireBot::CalGlobalBattleTable(const capi::GameState& state)
{
	MISS;
	std::vector<capi::Squad> mySquads;
	std::vector<capi::Squad> opSquads;

	for (auto S : state.entities.squads)
	{
		if (S.entity.player_entity_id == myId)mySquads.push_back(S);
		else if (S.entity.player_entity_id == opId)opSquads.push_back(S);
	}

	myBT = CalcBattleTable(mySquads);
	opBT = CalcBattleTable(opSquads);

	MISE;
	return true;
}

void FireBot::EchoBattleTable(BattleTable BT)
{
	MISS;
	std::stringstream ssLine;
	MISERROR("SIZE ->  #      S #      M #      L #     XL #");
	MISERROR("COUNTER  #####################################");
	for (unsigned int iCounter = 0; iCounter < 5; iCounter++)
	{
		ssLine.str("");
		if (iCounter == 0)     ssLine << "       S #";
		else if (iCounter == 1)ssLine << "       M #";
		else if (iCounter == 2)ssLine << "       L #";
		else if (iCounter == 3)ssLine << "      Xl #";
		else if (iCounter == 4)ssLine << "       ? #";

		for (unsigned int iSize = 0; iSize < 4; iSize++)
		{
			ssLine<< std::fixed << std::setw(7) << std::setfill(' ') << std::setprecision(0) << BT.SizeCounter[iSize][iCounter]<< " #";
		}
		MISERROR(ssLine.str());
	}
	MISE;
}