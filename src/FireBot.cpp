#define DF_Debug


// Memory Leak :-|
//What to do vs a WALL!!


#include "../incl/FireBot.h"
#include "../incl/CardBaseSMJ.h"
#include "../incl/Util.h"
#include "../incl/LOAD.h"

#include "../incl/FireBot_Thread.h"
#include "../incl/FireBot_AvoidArea.h"
#include "../incl/FireBot_RejectedComamand.h"
#include "../incl/FireBot_BattleTable.h"
#include "../incl/FireBot_CardPicker.h"
#include "../incl/FireBot_Util.h"

broker* (FireBot::Bro) = NULL;

bool run_FireBot(broker* Bro, unsigned short port, std::string sName)
{
	MISS;
	FireBot::learnBro(Bro);
	auto bot = FireBot(sName);
	run(bot, port);
	MISE;
	return true;
}

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
	iTier2Ready = -1;

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
	if (iTier2Ready > 0)iTier2Ready--;

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
		case Tier2:
			for (auto vv : sTier2(state))v.push_back(vv);
			break;
		case DefaultDef:
			for (auto vv : sDefaultDef(state))v.push_back(vv);
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

	std::vector<capi::Entity> vETemp = entitiesTOentity(myId, state.entities.token_slots);
	if (
		mapinfo.map == 1003 && // only for event
		iPanicDefCheck >= 0 &&
		vETemp.size() >0) 
		if(	Bro->U->pointsInRadius(entitiesTOentity(opId, state.entities.squads),
				capi::to2D(vETemp[0].position),
				100).size() >= 3
		&&	Bro->U->pointsInRadius(entitiesTOentity(myId, state.entities.squads),
				capi::to2D(vETemp[0].position),
				100).size() <= 1
		)ChangeStrategy(PanicDef, 0);
		
		


	switch (eStage)
	{
	case WaitForOP:
		if (state.current_tick >= 100)ChangeStrategy(GetUnit, 0);
		break;
	
	case SavePower:
	case DefaultDef:
		if ((int)state.players[imyPlayerIDX].power > iStageValue)bStage = true;
		break;
		
	case PanicDef:
		if(vETemp.size() > 0)if (Bro->U->pointsInRadius(entitiesTOentity(opId, state.entities.squads),
			capi::to2D(vETemp[0].position),
			100).size() == 0)ChangeStrategy(DisablePanicDef, 0);
		if (iPanicDefCheck >= 10)
		{
			iPanicDefCheck = -600;
			ChangeStrategy(Fight, 0);
			MISD("Disable PanicDef");
		}
			break;
	case GetUnit:  //Manuel Stageing with eNextStage	
		if (entitiesTOentity(myId, state.entities.squads).size() > 0)bStage = true;
		break;
		
	case DisablePanicDef:
//		if(entitiesTOentity(myId, state.entities.barrier_sets).size() == 0)bStage = true;
//		break;
	case Tier2:
		if((int)state.players[imyPlayerIDX].orbs.all >= 2)bStage = true;
		if (iTier2Ready == 0)
		{
			iTier2Ready--;
			bStage = true;
		}
		break;
	case BuildWell:
	case SpamBotX: //Never Stages xD
	case Fight: 
		if (entitiesTOentity(myId, state.entities.squads).size() == 0)ChangeStrategy(DefaultDef, 150);
		for (auto B : entitiesTOentity(myId, state.entities.power_slots, state.entities.token_slots))
			if(Bro->U->SquadsInRadius(opId, state.entities.squads, capi::to2D(B.position), 25).size() > 0)
				ChangeStrategy(DefaultDef, 75);
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

/// STAGES ///
std::vector<capi::Command> FireBot::sWaitForOP(const capi::GameState& state)
{
	MISS;
	auto vReturn = std::vector<capi::Command>();
	if (state.entities.squads.size() == 0)return vReturn;
	CalGlobalBattleTable(state);

	vReturn.push_back(MIS_CommandProduceSquad(CardPickerFromBT(opBT, Swift, (int)state.players[imyPlayerIDX].orbs.all),
		capi::to2D(entitiesTOentity(myId, state.entities.token_slots)[0].position))); // Index 0 OK becaus opener

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
	int iCard = CardPickerFromBT(opBT, None, (int)state.players[imyPlayerIDX].orbs.all);

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
	int iCard = CardPickerFromBT(opBT, None, (int)state.players[imyPlayerIDX].orbs.all);

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

			if (state.current_tick % 5 == 0 || NextCardSpawn == -1)NextCardSpawn = CardPickerFromBT(CalcBattleTable(Bro->U->SquadsInRadius(opId, state.entities.squads, capi::to2D(S.entity.position), FightRange)), None, (int)state.players[imyPlayerIDX].orbs.all);

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


		for (auto vv : IdleToFight(state))vReturn.push_back(vv);
	}
	MISE;
	return vReturn;
}

std::vector<capi::Command> FireBot::sPanicDef(const capi::GameState& state)
{
	MISS;
	//bool OnWall;
	auto vReturn = std::vector<capi::Command>();
	//capi::Entity myOrb = entitiesTOentity(myId, state.entities.token_slots)[0];
	std::vector<capi::Entity> myOrb = entitiesTOentity(myId, state.entities.token_slots);
	if (myOrb.size() == 0) return vReturn;
	
	
	if (iWallReady == -1)
	{
		//Find My Wall
		std::vector<capi::Entity> barrier =
			Bro->U->pointsInRadius(entitiesTOentity(0, state.entities.barrier_sets),
				capi::to2D(myOrb[0].position),
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
	unsigned int iArchers = CardPicker(99, 99, Archer, (int)state.players[imyPlayerIDX].orbs.all);
	if (state.players[imyPlayerIDX].power > SMJDeck[iArchers].powerCost)
	{
		vReturn.push_back(MIS_CommandProduceSquad(iArchers, capi::to2D(myOrb[0].position)));
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


	
	for (auto S : Bro->U->pointsInRadius(entitiesTOentity(myId, state.entities.squads), capi::to2D(myOrb[0].position), 50))
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

std::vector<capi::Command> FireBot::sTier2(const capi::GameState& state)
{
	MISS;

	auto vReturn = std::vector<capi::Command>();

	//Tier 2 good idea? or wait -> Def + 50 Power
	//Battle Tabel + Power (+ Void)

	//find T2 Spot

	//Build T2 if still good idea

	//check if orb is in cosntruction, if yes STAGE

	iTier2Ready = 400;

	bStage = true;

	MISE;
	return vReturn;
}

std::vector<capi::Command> FireBot::sDefaultDef(const capi::GameState& state)
{
	MISS;

	auto vReturn = std::vector<capi::Command>();
	capi::Entity A;
	capi::Entity B;
	BattleTable myBT_Area;
	BattleTable opBT_Area;
	float fDisntanc;

	std::vector < capi::Squad> opUnits;
	std::vector < capi::Squad> myUnits;

	auto eBase = entitiesTOentity(myId, state.entities.power_slots, state.entities.token_slots);

	//If units fare between 50 and 100 Pull bakc
	// >100 will be send on a atack later
	for (auto eSquat : entitiesTOentity(myId, state.entities.squads))
	{
		fDisntanc = Bro->U->CloseCombi({ eSquat }, eBase, A, B);
		if (fDisntanc > 50 && fDisntanc < 100)
			vReturn.push_back(MIS_CommandGroupGoto({ A.id }, capi::to2D(B.position), capi::WalkMode_Normal));
	}
	
	for (auto EE : eBase)
	{
		//when no OP is close the get all units in range to get closer + NEXT
		opUnits = Bro->U->SquadsInRadius(opId, state.entities.squads, capi::to2D(EE.position), 50);
		myUnits = Bro->U->SquadsInRadius(myId, state.entities.squads, capi::to2D(EE.position), 50);
		if (opUnits.size() == 0)
		{
			for (auto EEE : myUnits)
				vReturn.push_back(MIS_CommandGroupGoto({ EEE.entity.id }, capi::to2D(EE.position), capi::WalkMode_Normal));

			continue;
		}

		//If OP is close, calk battel tabels and may spwan renforcmenc
		//or when base has low HP
		else
		{
			myBT_Area = CalcBattleTable(myUnits);
			opBT_Area = CalcBattleTable(opUnits);

			if (MoreUnitsNeeded(myBT_Area, opBT_Area) || 
				GetAspect(EE, capi::AspectCase::Health) < 1500)
			{
				vReturn.push_back(
					MIS_CommandProduceSquad(
						CardPickerFromBT(opBT_Area, None, (int)state.players[imyPlayerIDX].orbs.all),
						capi::to2D(EE.position)));
			}
		}
	}

	//Ideal Units do stuff
	for (auto vv : IdleToFight(state))vReturn.push_back(vv);
				
	MISE;
	return vReturn;
}