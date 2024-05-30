#define DF_Debug


// Memory Leak :-|
// Ofset rjected command
//What to do vs a WALL!!


#include "../incl/FireBot.h"
#include "../incl/CardBaseSMJ.h"
#include "../incl/Util.h"
#include "../incl/LOAD.h"

broker* (FireBot::Bro) = NULL;

void FireBot::PrepareForBattle(const capi::MapInfo& mapInfo, const capi::Deck& deck)
{
	MISS;
	MISD("PrepareForBattle: " + std::to_string(mapInfo.map) + " with " + deck.name);
	mapinfo = mapInfo;
	
	MaxAvoidID = 0;
	NextCardSpawn = -1;
	iWallReady = -1;
	iMyWells = -1;
	iPanicDefCheck = 0;

//if mapInfo.map == 1003 -> no Panic Def
	eStage = NoStage;
	bStage = true;

	vAvoid.clear();
	RejectedComamandChecklist.clear();
	vSaveUnit.clear();
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

	EruptionPos = -1;
	for (unsigned int i = 0; i < SMJDeck.size(); i++) if (SMJDeck[i].cardName == "Eruption")EruptionPos = i;
	if (EruptionPos == -1)
	{
		MISD("No Eruption in deck");		
		CoolEruptionTest.s = false;
	}
	else CoolEruptionTest.s = true;
	GlobalBattleTable.s = true;
	InstantRepair.s = true;
		
	MISE;
}


std::vector<capi::Command> FireBot::Tick(const capi::GameState& state)
{		
	auto v = std::vector<capi::Command>();
	auto vTemp = std::vector<capi::Command>();
#ifdef MIS_DEBUG

	if(Bro->L->AllTick)	MISD(std::to_string(eStage) + "#" + Bro->sTime(state.current_tick) + "#" + std::to_string(state.current_tick) + "#" + std::to_string(state.players[imyPlayerIDX].power));

	if (Bro->sComand != "")
	{
		if (Bro->sComand == "?")
		{
			MISERROR("------------------------------");
			MISERROR("# dump     = Save tick        ");
			MISERROR("# bt       = Print BattleTable");
			MISERROR("# stage    = Go to next stage ");
			MISERROR("# st       = echo strategy    ");
			MISERROR("------------------------------");
			Bro->sComand = "";
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
			Bro->sComand = "";
		}

		if (Bro->sComand == "stage")
		{
			bStage = true;
			Bro->sComand = "";
		}

		if (Bro->sComand == "st")
		{
			for (auto S : Bro->L->vStrategy)MISERROR(std::to_string(S.first) + " - " + std::to_string(S.second));
			Bro->sComand = "";
		}

		
		//Bro->sComand = "";
	}
#endif

	for (auto r : state.rejected_commands)
	{
		MISD("rejected Player: " + std::to_string(r.player));
		MISD("reason         : " + Bro->U->switchCommandRejectionReason(r.reason));
		MISD("command        : " + Bro->U->switchCommand(r.command));

		if (r.player == myId)
		{
			switch (r.reason.variant_case)
			{
			case capi::CommandRejectionReasonCase::CardRejected:
				if(r.command.variant_case== capi::CommandCase::ProduceSquad)
					for (auto vv : Handel_CardRejected_ProduceSquad(r.command))v.push_back(vv);
				break;
			}
		}
		
	}
	
	unsigned int iRejectEnd = v.size();
	
	if (iSkipTick > 0)
	{
		MISD("SKIP: " + std::to_string(iSkipTick) );
		iSkipTick--;
		return v;
	}
	if (iWallReady >= 0)iWallReady--;
	if (iPanicDefCheck < 0)iPanicDefCheck++;

	if (state.current_tick % 10)Stage(state);

	
		
	///////////////Timing critical ////////////////
	//WELL KILLER
	if (Bro->L->WellKiller && EruptionPos != -1)
		if(WellKiller(v, entitiesTOentity(opId, state.entities.power_slots)))
			return v;
	
	//Avoid Spells
	if (Bro->L->AvoidArea)
	{
		//Find New Stuff
		FindAvoidArea(state);

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
	
	if (Bro->L->BattleTable && GlobalBattleTable.s && state.current_tick % 20 == 1)
	{
		GlobalBattleTable.fb = std::async(&FireBot::CalGlobalBattleTable, this, state);
		GlobalBattleTable.s = false; // dont start again
	}
	
	if (Bro->L->InstantRepair && InstantRepair.s && state.current_tick % 20 == 1)
	{
		InstantRepair.fc = std::async(&FireBot::InstantRepairFunction, this, state);
		InstantRepair.s = false; // dont start again
	}

	////////////////Starters ////////////////////
	
	switch (eStage)
	{
			//Wait for spawn for perfect counter
		case WaitForOP:
			for (auto vv : sWaitForOP(state))v.push_back(vv);
			break;
			//Get Unit Close to OP Well/Orb
		case GetUnit:
			for (auto vv : sGetUnit(state))v.push_back(vv);
			break;
			//Get a Well
		case BuildWell:
			for (auto vv : sBuildWell(state))v.push_back(vv);
			break;
		case SpamBotX:
			for (auto vv : sSpamBotX(state))v.push_back(vv);
			break;
		case Fight:
			for (auto vv : sFight(state))v.push_back(vv);
			break;
		case PanicDef:
			for (auto vv : sPanicDef(state))v.push_back(vv);
			break;
		case DisablePanicDef:
			for (auto vv : sDisablePanicDef(state))v.push_back(vv);
			break;
	}


	/////////////Cleanups////////////////
	if (Bro->L->AvoidArea)if (vAvoid.size() > 0)RemoveFromMIS_AvoidArea(state.current_tick);
	if (state.current_tick % 100 == 1)RemoveFromSaveUnit(state);
	CleanUpRejectedComamandChecklist();

	/////////////THREADS////////////////
	if(Bro->L->UnitEruption && CoolEruptionTest.s == false)
		if (CoolEruptionTest.fc.wait_for(0ms) == std::future_status::ready)
		{
			for (auto vv : CoolEruptionTest.fc.get())v.push_back(vv);
			CoolEruptionTest.s = true;
		}
	if (Bro->L->BattleTable && GlobalBattleTable.s == false)
		if (GlobalBattleTable.fb.wait_for(0ms) == std::future_status::ready)
			GlobalBattleTable.s = true;
	if (Bro->L->InstantRepair && InstantRepair.s == false)
		if (InstantRepair.fc.wait_for(0ms) == std::future_status::ready)
		{
			for (auto vv : InstantRepair.fc.get())v.push_back(vv);
			InstantRepair.s = true;
		}

	//Fight -> pick best unit to kill (counters)

	// Recet orders -> Prduce Squat -> ofset in 4 directions (loop)

	//Remove dobbel orders
	//dont cast unit int avoid area

	//Iff rooted skip order to move out of avoid

#ifdef MIS_DEBUG
	if (Bro->sComand == "dumpx" )
	{
		auto effect_json = nlohmann::json(v).dump();
		std::ofstream file("DUMPX_at_" + std::to_string(state.current_tick) + ".json");
		file << effect_json << std::endl;
		file.close();
		Bro->sComand = "";
	}
#endif

	for (unsigned int i = iRejectEnd; i < v.size(); i++)
		RejectedComamandChecklist.push_back(v[i]);
	return v;
}

std::vector<capi::Command> FireBot::CoolEruption(const capi::GameState& state)
{
	MISS;

	auto vReturn = std::vector<capi::Command>();

	unsigned int iUnitCount;
	unsigned int iUnitCountH;

	std::vector<capi::Squad> vSquad;
	for (auto U : state.entities.squads)
	{
		
		if (U.entity.player_entity_id != opId)continue;

		vSquad = Bro->U->SquadsInRadius(opId, state.entities.squads, capi::to2D(U.entity.position), 10);
		if (vSquad.size() >= 3)
		{

			/// Check if Units Is Skyfire and <300 HP -> Erupt

			iUnitCount = 0;
			iUnitCountH = 0;
			for (auto S : vSquad)
			{
				iUnitCount++;
				for (auto A : S.entity.aspects)
				{
					if (A.variant_case == capi::AspectCase::Health)
						if (A.variant_union.health.current_hp <= 300)iUnitCountH++;
				}
			}
			if (iUnitCountH >= 1 && iUnitCount >= 3
				|| iUnitCountH >= 2 && iUnitCount >= 2)
			{
				MISD("FIRE !!!!");

				auto spell = capi::CommandCastSpellGod();
				spell.card_position = EruptionPos;
				spell.target = capi::SingleTargetLocation(capi::to2D(U.entity.position));
				vReturn.push_back(capi::Command(spell));
				break; //only for one unit not all 3
			}
		}
	}

	MISE;
	return vReturn;	
}

std::vector<capi::Command> FireBot::InstantRepairFunction(const capi::GameState& state)
{
	MISS;

	auto vReturn = std::vector<capi::Command>();			
	for (auto E : entitiesTOentity(myId, state.entities.power_slots, state.entities.token_slots))
		for (auto A : E.aspects)
			if (A.variant_case == capi::AspectCase::Health)
				if (A.variant_union.health.current_hp != A.variant_union.health.cap_current_max)
					vReturn.push_back(MIS_CommandRepairBuilding(E.id));
	//MISD(vReturn.size());
	MISE;
	return vReturn;
}

int FireBot::CardPicker(unsigned int opSize, unsigned int opCounter, CardPickCrit Crit)
{
	MISS;
	
	
	for (unsigned int i = 0; i < SMJDeck.size(); i++)
	{
		if (   (SMJDeck[i].offenseType == opSize || opSize == 99) 
			&& (SMJDeck[i].defenseType != opCounter || opCounter==99))
		{
			switch (Crit)
			{
			case Swift:
				for (Ability A : SMJDeck[i].abilities)
					if (A.abilityIdentifier == "Swift")
					{
						MISEA("Swift: " + SMJDeck[i].cardName);
						return i;
					}
				break;
			case Archer:
				if (SMJDeck[i].unitClass == "Archer")
				{
					MISEA("Archer: " + SMJDeck[i].cardName);
					return i;
				}				
				break;
			case NotS:
				if (SMJDeck[i].defenseType == 0)break;
			case NotM:
				if (SMJDeck[i].defenseType == 1)break;
			case NotL:
				if (SMJDeck[i].defenseType == 2)break;

			case None:
			default:
				MISEA("Default: " + SMJDeck[i].cardName)
				return i;
			}
			
		}				
	}
		
	MISEA("No Card");
	return -1;
}

int FireBot::CardPickerFromBT(BattleTable BT, CardPickCrit Crit)
{
	MISS;
	int iReturn = -1;
	unsigned int MaxSize = 0;
	unsigned int MaxCounter = 0;
	unsigned int MaxHealth = 0;

	for (unsigned int iCounter = 0; iCounter < 5; iCounter++)
		for (unsigned int iSize = 0; iSize < 4; iSize++)
			if (MaxHealth < BT.SizeCounter[iSize][iCounter])
			{
				MaxSize = iSize;
				MaxCounter = iCounter;
				MaxHealth = BT.SizeCounter[iSize][iCounter];							
			}

	//Maybe check my own Battle Tabble if i have egnaut units of that type???

	if ((iReturn = CardPicker(MaxSize, MaxCounter, Crit)) == -1)    //Perfect Counter
		if ((iReturn = CardPicker(MaxSize, 9, Crit)) == -1)   //Counter of any Size
			if ((iReturn = CardPicker(MaxSize, MaxCounter, None)) == -1)    //Perfect Counter, not Crits
				if ((iReturn = CardPicker(MaxSize, 9, None)) == -1)   //Counter of any Size, not Crits
					iReturn = -1; // I dont have a counter card???

	// If size is XL do XYZ


	if (iReturn == -1)
	{
		unsigned int iAvoidSize = MaxSize;
		MaxSize = 0;
		MaxCounter = 0;
		MaxHealth = 0;
		for (unsigned int iCounter = 0; iCounter < 5; iCounter++)
			for (unsigned int iSize = 0; iSize < 4; iSize++)
				if (MaxHealth < BT.SizeCounter[iSize][iCounter] && iAvoidSize != iSize)
				{
					MaxSize = iSize;
					MaxCounter = iCounter;
					MaxHealth = BT.SizeCounter[iSize][iCounter];
				}
		if ((iReturn = CardPicker(MaxSize, MaxCounter, Crit)) == -1)    //Perfect Counter
			if ((iReturn = CardPicker(MaxSize, 9, Crit)) == -1)   //Counter of any Size
				if ((iReturn = CardPicker(MaxSize, MaxCounter, None)) == -1)    //Perfect Counter, not Crits
					if ((iReturn = CardPicker(MaxSize, 9, None)) == -1)   //Counter of any Size, not Crits
						iReturn = -1; // I dont have a counter card???
	}


	if (iReturn == -1)iReturn = 0; // Play card 1
	MISE;
	return iReturn;
}



bool run_FireBot(broker* Bro, unsigned short port, std::string sName)
{
	MISS;
	FireBot::learnBro(Bro);
	auto bot = FireBot(sName);
	run(bot, port);
	MISE;
	return true;
}

bool FireBot::WellKiller(std::vector<capi::Command> &vMain, std::vector<capi::Entity> Wells)
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
					spell.card_position = EruptionPos;
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

void FireBot::ChangeStrategy(Stages _Stage, int _Value)
{
	if (Bro->L->vStrategy.size() > 0) if( Bro->L->vStrategy[0].first == _Stage)return;
	Bro->L->vStrategy.insert(Bro->L->vStrategy.begin(), std::make_pair(_Stage, _Value));
	bStage = true;
}

bool FireBot::Stage(const capi::GameState& state)
{
	MISS;

	if (bStage && Bro->L->vStrategy.size() >0)
	{
		MISD("Old Stage: " + std::to_string(eStage) + "(" + std::to_string(iStageValue) + ")");
		eStage = (Stages)Bro->L->vStrategy[0].first;
		iStageValue = Bro->L->vStrategy[0].second;
		bStage = false;
		Bro->L->vStrategy.erase(Bro->L->vStrategy.begin());
		MISD("New Stage: " + std::to_string(eStage) + "(" + std::to_string(iStageValue) + ")");
		if(Bro->L->vStrategy.size() == 0)Bro->L->vStrategy.insert(Bro->L->vStrategy.begin(), std::make_pair(Fight, 0));
		return true;
	}

	if (
		mapinfo.map == 1003 && // only for event
		iPanicDefCheck >= 0 &&
		Bro->U->pointsInRadius(entitiesTOentity(opId, state.entities.squads),
		capi::to2D(entitiesTOentity(myId, state.entities.token_slots)[0].position),
		100).size() >= 3
		&&
		Bro->U->pointsInRadius(entitiesTOentity(myId, state.entities.squads),
			capi::to2D(entitiesTOentity(myId, state.entities.token_slots)[0].position),
			100).size() <= 1
		)ChangeStrategy(PanicDef, 0);
		
		
	
		







	switch (eStage)
	{
	case WaitForOP:
		if (state.current_tick >= 100)ChangeStrategy(GetUnit, 0);
		break;
	
	case SavePower:
		//MISD(std::to_string((int)state.players[imyPlayerIDX].power) + " > " + std::to_string(iStageValue));
		
		if ((int)state.players[imyPlayerIDX].power > iStageValue)bStage = true;
		break;

	
		
	case PanicDef:
		if (Bro->U->pointsInRadius(entitiesTOentity(opId, state.entities.squads),
			capi::to2D(entitiesTOentity(myId, state.entities.token_slots)[0].position),
			100).size() == 0)ChangeStrategy(DisablePanicDef, 0);
		if (iPanicDefCheck >= 10)
		{
			iPanicDefCheck = -600;
			ChangeStrategy(Defend, 0);
			MISD("Disable PanicDef");
		}
			break;
	case GetUnit:  //Manuel Stageing with eNextStage	
		if (entitiesTOentity(myId, state.entities.squads).size() > 0)bStage = true;
		break;
	case Defend:
		ChangeStrategy(Fight, 0);
		break;
		
	case DisablePanicDef:
//		if(entitiesTOentity(myId, state.entities.barrier_sets).size() == 0)bStage = true;
//		break;
	case BuildWell:
	case SpamBotX: //Never Stages xD
	case Fight: //Never Stages xD
		break;
	}
	
	//bStage = false;
	//if units close to my Well / Orb -> Def.

	//if nothing to do - maybe Save Power

	//If time is right Well / Orb

	//if Def is done -> Atack
	//Or when power is saved
	//or when OP build well
	
	MISE;
	return false;;
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

void FireBot::RemoveFromSaveUnit(const capi::GameState& state)
{
	MISS;
	bool bDel;
	std::vector < capi::Entity> MyUnits = entitiesTOentity(myId, state.entities.squads);
	for (std::vector<capi::EntityId>::iterator it = vSaveUnit.begin(); it != vSaveUnit.end();)
	{
		bDel = true;
		for (auto U : MyUnits)if ((*it) == U.id) bDel = false;
	
		if (bDel)it = vSaveUnit.erase(it);
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


std::vector<capi::Command> FireBot::sWaitForOP(const capi::GameState& state)
{
	MISS;
	auto vReturn = std::vector<capi::Command>();
	if (state.entities.squads.size() == 0)return vReturn;
	CalGlobalBattleTable(state);

	vReturn.push_back(MIS_CommandProduceSquad(CardPickerFromBT(opBT, Swift), 
		capi::to2D(entitiesTOentity(myId, state.entities.token_slots)[0].position)));

	iSkipTick = 50; // Wait till spwn is done
	bStage = true;
	MISE;
}

std::vector<capi::Command> FireBot::sBuildWell(const capi::GameState& state)
{
	MISS;

	auto vReturn = std::vector<capi::Command>();
	
	if (iMyWells == -1)
		iMyWells = entitiesTOentity(myId, state.entities.power_slots).size();
	if (iMyWells != -1 && iMyWells != entitiesTOentity(myId, state.entities.power_slots).size())
	{
		iMyWells = -1;
		bStage = true;
		MISD("!!! bStage !!!");
		MISEA("Build");
		return vReturn;
	}

	// We need a Unit to go to the well
	if (entitiesTOentity(myId, state.entities.squads).size() == 0)
	{
		ChangeStrategy(GetUnit, 0);
		return vReturn;
	}

	capi::Entity A;
	capi::Entity B;
	float fDistanc = 0;

	fDistanc = Bro->U->CloseCombi(entitiesTOentity(myId, state.entities.squads),
		entitiesTOentity(0, state.entities.power_slots), A, B);

	if (A.player_entity_id == myId)
	{
		vReturn.push_back(MIS_CommandGroupGoto({ A.id }, capi::to2D(B.position), capi::WalkMode_Normal));
		if (fDistanc < CastRange && state.players[imyPlayerIDX].power >= 100)
			vReturn.push_back(capi::Command(MIS_CommandPowerSlotBuild(B.id)));			
	}
	MISE;
	return vReturn;
}

std::vector<capi::Command> FireBot::sGetUnit(const capi::GameState& state)
{
	MISS;
	auto vReturn = std::vector<capi::Command>();
	int iCard = CardPickerFromBT(opBT, None);

	if (SMJDeck[iCard].powerCost > state.players[imyPlayerIDX].power)
	{
		MISEA("No Power: " + std::to_string(SMJDeck[iCard].powerCost) + ">" + std::to_string(state.players[imyPlayerIDX].power));
		return vReturn;
	}

	capi::Entity A;
	capi::Entity B;

	Bro->U->CloseCombi(
		entitiesTOentity(myId, state.entities.power_slots, state.entities.token_slots, state.entities.squads, state.entities.buildings),
		entitiesTOentity(opId, state.entities.power_slots, state.entities.token_slots, state.entities.squads),
		A, B);

	vReturn.push_back(MIS_CommandProduceSquad(iCard,
		Bro->U->Offseter(capi::to2D(A.position), capi::to2D(B.position), CastRange)));

	iSkipTick = 50; // Wait till spwn is done
	bStage = true;
	MISE;
}



std::vector<capi::Command> FireBot::sSpamBotX(const capi::GameState& state)
{
	MISS;

	auto vReturn = std::vector<capi::Command>();

	capi::Entity A;
	capi::Entity B;	
	int iCard = CardPickerFromBT(opBT, None);	

	float fDistanc = Bro->U->CloseCombi(entitiesTOentity(myId, state.entities.squads, state.entities.buildings, state.entities.power_slots, state.entities.token_slots),
		entitiesTOentity(opId, state.entities.power_slots, state.entities.token_slots), A, B);

	if (SMJDeck[iCard].powerCost > state.players[imyPlayerIDX].power)
	{
		//Cant Spawn insice the base (collition)
		if (fDistanc > CastRange * 2) fDistanc = CastRange;
		else fDistanc = CastRange * -1;

		vReturn.push_back(MIS_CommandProduceSquad(iCard,
			Bro->U->Offseter(capi::to2D(A.position), capi::to2D(B.position), fDistanc)));
	}

	for (auto E : entitiesTOentity(myId, state.entities.squads))	
		if (E.job.variant_case == capi::JobCase::Idle)		
			vReturn.push_back(MIS_CommandGroupGoto({ E.id }, capi::to2D(B.position), capi::WalkMode_Normal));

	MISE;
	return vReturn;
}

std::vector<capi::Command> FireBot::sFight(const capi::GameState& state)
{
	MISS;
	auto vReturn = std::vector<capi::Command>();

	std::vector<capi::Squad> mySquat = Bro->U->FilterSquad(myId, state.entities.squads);

	capi::Entity A;
	capi::Entity B;
	float fDistanc = Bro->U->CloseCombi(entitiesTOentity(myId, state.entities.squads),
		entitiesTOentity(opId, state.entities.power_slots, state.entities.token_slots, state.entities.squads, state.entities.buildings), A, B);

	//NO unit? get any unit
	if (mySquat.size() == 0)
	{
		ChangeStrategy(GetUnit, 0);
		MISEA("No Units change Stage");
		return vReturn;
	}
	else for (auto S : mySquat)
	{		
		if (S.entity.job.variant_case == capi::JobCase::AttackSquad || fDistanc < CastRange * 3) // Unit in Combet pr near
		{
			if (CARD_ID_to_SMJ_CARD(S.card_id).attackType == 1)
			if (std::find(vSaveUnit.begin(), vSaveUnit.end(), S.entity.id) == vSaveUnit.end())
				for (auto F : state.entities.figures)
					if(F.squad_id == S.entity.id)
						for (auto Asp : F.entity.aspects)
							if (Asp.variant_case == capi::AspectCase::Health)
								if (Asp.variant_union.health.current_hp / Asp.variant_union.health.cap_current_max < 0.3)
									{
										vSaveUnit.push_back(S.entity.id);
										Bro->U->CloseCombi({ S.entity },
											entitiesTOentity(opId, state.entities.power_slots, state.entities.token_slots, state.entities.squads), A, B);
										vReturn.push_back(MIS_CommandGroupGoto({ S.entity.id },
											Bro->U->Offseter(capi::to2D(A.position), capi::to2D(B.position), CastRange * -1), capi::WalkMode_Force));
									}

			if (state.current_tick % 5 == 0 || NextCardSpawn == -1)NextCardSpawn = CardPickerFromBT(CalcBattleTable(Bro->U->SquadsInRadius(opId, state.entities.squads, capi::to2D(S.entity.position), FightRange)), None);

			if (SMJDeck[NextCardSpawn].powerCost < state.players[imyPlayerIDX].power)
			{
				capi::Position2D SpawnPos;

				//if well near by go there
				std::vector<capi::Entity> UnDazed = Bro->U->pointsInRadius(entitiesTOentity(myId, state.entities.power_slots, state.entities.token_slots), capi::to2D(S.entity.position), 25);
				if (UnDazed.size() > 0)
				{
					MISD("Spawn undazed");
					SpawnPos = Bro->U->Offseter(capi::to2D(UnDazed[0].position), capi::to2D(S.entity.position), CastRange);

				}
				//if archer go away
				else if (SMJDeck[NextCardSpawn].attackType == 1) //ranged
				{
					MISD("Spawn Range Distance");
					Bro->U->CloseCombi({ S.entity },
						entitiesTOentity(opId, Bro->U->SquadsInRadius(opId, state.entities.squads, capi::to2D(S.entity.position), FightRange)),
						A, B);
					SpawnPos = Bro->U->Offseter(capi::to2D(B.position), capi::to2D(A.position), CastRange); // ArcherRange);
				}
				//if melee go near
				else
				{
					MISD("Spawn Near");
					fDistanc = Bro->U->CloseCombi(entitiesTOentity(myId, Bro->U->SquadsInRadius(myId, state.entities.squads, capi::to2D(S.entity.position), 75)),
						entitiesTOentity(opId, Bro->U->SquadsInRadius(opId, state.entities.squads, capi::to2D(S.entity.position), FightRange)), A, B);
					if (fDistanc > CastRange)fDistanc = CastRange;
					SpawnPos = Bro->U->Offseter(capi::to2D(A.position), capi::to2D(B.position), fDistanc - 1, -1); //a bit offset
				}
				vReturn.push_back(MIS_CommandProduceSquad(NextCardSpawn, SpawnPos));
			}
		}

		if (S.entity.job.variant_case == capi::JobCase::Idle)
		{
			//Units Close By
			std::vector<capi::Squad> STemp = Bro->U->SquadsInRadius(opId, state.entities.squads, capi::to2D(S.entity.position), FightRange);
			if (STemp.size() > 0)
			{
				vReturn.push_back(MIS_CommandGroupAttack({ S.entity.id }, STemp[0].entity.id));
			}
			else
			{
				Bro->U->CloseCombi({ S.entity },
					entitiesTOentity(opId, state.entities.power_slots, state.entities.token_slots, state.entities.squads), A, B);
				vReturn.push_back(MIS_CommandGroupGoto({ S.entity.id }, capi::to2D(B.position), capi::WalkMode_Normal));
			}
		}
	}
	MISE;
	return vReturn;
}


Card FireBot::CARD_ID_to_SMJ_CARD(capi::CardId card_id)
{
	//MISS
	//in my Deck?
	for (auto C : SMJDeck)if (C.officialCardIds == card_id % 1000000)return C;
	//in op Deck?
	for (auto C : SMJDeckOP)if (C.officialCardIds == card_id % 1000000)return C;
	//Add to OP Deck;
	SMJDeckOP.push_back(Bro->J->CardFromJson(card_id % 1000000));
	return SMJDeckOP[SMJDeckOP.size() - 1];
	//MISE;
}


std::vector<capi::Command> FireBot::sPanicDef(const capi::GameState& state)
{
	MISS;
	//bool OnWall;
	auto vReturn = std::vector<capi::Command>();
	capi::Entity myOrb = entitiesTOentity(myId, state.entities.token_slots)[0];
	
	if (iWallReady == -1)
	{
		//Find My Wall
		std::vector<capi::Entity> barrier =
			Bro->U->pointsInRadius(entitiesTOentity(0, state.entities.barrier_sets),
				capi::to2D(myOrb.position),
				50);

		if (barrier.size() == 1)
		{
			//Build Wall
			vReturn.push_back(MIS_CommandBarrierBuild(barrier[0].id));
			iSkipTick = 10;
			MISEA("First Build Wall");
			iPanicDefCheck++;
			return vReturn;
		}
		else
		{
			iWallReady = -2;
		}
	}
	
	//Spawn Archers
	unsigned int iArchers = CardPicker(99, 99, Archer);
	if (state.players[imyPlayerIDX].power > SMJDeck[iArchers].powerCost)
	{
		vReturn.push_back(MIS_CommandProduceSquad(iArchers, capi::to2D(myOrb.position)));
	}

	//Archers on the Wall
	
	std::vector < capi::Entity> myBarrier = entitiesTOentity(myId, state.entities.barrier_modules);
	capi::Position GatePos;
	//Remove Gate from Vector
	for (unsigned int i = 0; i < myBarrier.size() ;i++)
		for(auto A: myBarrier[i].aspects)
			if (A.variant_case == capi::AspectCase::BarrierGate)
			{
				GatePos = myBarrier[i].position;
                myBarrier.erase(myBarrier.begin() + i);
                goto GateLoopOut;
            }	
    GateLoopOut:


	if (myBarrier.size() == 0)
	{
		MISEA("WHERE IS MY WALL!!!");
		
		return vReturn;
	}


	
	for (auto S : Bro->U->pointsInRadius(entitiesTOentity(myId, state.entities.squads), capi::to2D(myOrb.position), 50))
		for (auto A : S.aspects)
			if (A.variant_case == capi::AspectCase::MountBarrier)		
				if (A.variant_union.mount_barrier.state.variant_case == capi::MountStateCase::Unmounted)
				{
					capi::Entity A;
					capi::Entity B;

					Bro->U->CloseCombi({ S }, myBarrier, A, B);
					vReturn.push_back(MIS_CommandGroupEnterWall({ S.id }, B.id));
				}

	
	// Get Idle Units to help
	for (auto E : entitiesTOentity(myId, state.entities.squads))
		if(Bro->U->distance(capi::to2D(E.position), capi::to2D(GatePos))>25)
		if (E.job.variant_case == capi::JobCase::Idle)
			vReturn.push_back(MIS_CommandGroupGoto({ E.id }, capi::to2D(GatePos), capi::WalkMode_Normal));
	
	MISE;
	return vReturn;
}




std::vector<capi::Command> FireBot::sDisablePanicDef(const capi::GameState& state)
{
	MISS;
	
	auto vReturn = std::vector<capi::Command>();
	for (auto B : entitiesTOentity(myId, state.entities.barrier_sets))
		vReturn.push_back(MIS_CommandGroupKillEntity({ B.id }));

	iWallReady = 70 * 10; //ready in ~65 sec
	iPanicDefCheck = 0;

	bStage = true;

	MISE;
	return vReturn;
}


void FireBot::CleanUpRejectedComamandChecklist()
{
	MISS;
	for (std::vector<MIS_RejectCheck>::iterator it = RejectedComamandChecklist.begin(); it != RejectedComamandChecklist.end();)
		if ((*it).rejected == false )it = RejectedComamandChecklist.erase(it);
		else  ++it;
	MISE;
}

std::vector<capi::Command> FireBot::Handel_CardRejected_ProduceSquad(capi::Command cIN) //, std::vector<capi::Command>& addHere)
{
	MISS;	

	std::vector<capi::Command> vReturn;

	for (unsigned int i = 0 ; i < RejectedComamandChecklist.size();i++)
	{
		if (RejectedComamandChecklist[i].lastCommand.variant_case == cIN.variant_case)
		{
			if (RejectedComamandChecklist[i].lastCommand.variant_union.produce_squad.card_position == cIN.variant_union.produce_squad.card_position
				&& RejectedComamandChecklist[i].lastCommand.variant_union.produce_squad.xy.x == cIN.variant_union.produce_squad.xy.x
				&& RejectedComamandChecklist[i].lastCommand.variant_union.produce_squad.xy.y == cIN.variant_union.produce_squad.xy.y)
			{
				RejectedComamandChecklist[i].rejected = true;
				RejectedComamandChecklist[i].lastCommand = RejectedComamandChecklist[i].orgCommand;
				switch (RejectedComamandChecklist[i].retry)
				{
				case 0:
					RejectedComamandChecklist[i].lastCommand.variant_union.produce_squad.xy.x += 10;
					break;
				case 1:
					RejectedComamandChecklist[i].lastCommand.variant_union.produce_squad.xy.y += 10;
					break;
				case 2:
					RejectedComamandChecklist[i].lastCommand.variant_union.produce_squad.xy.x -= 10;
					break;
				case 3:
					RejectedComamandChecklist[i].lastCommand.variant_union.produce_squad.xy.y -= 10;
					break;
				default:
					RejectedComamandChecklist[i].rejected = false;
					break;
				}
				RejectedComamandChecklist[i].retry++;

				if (RejectedComamandChecklist[i].rejected)vReturn.push_back(RejectedComamandChecklist[i].lastCommand);
			}
		}
		
	}

	MISE;
	return vReturn;
}