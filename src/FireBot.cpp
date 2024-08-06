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
	iTierReady = -1;

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
	lState = state;
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
	if (iTierReady > 0)iTierReady--;

	if (state.current_tick % 10)Stage();

	
		
	///////////////Timing critical ////////////////
	//WELL KILLER
	if (Bro->L->WellKiller && EruptionPos != -1)
		if(WellKiller(v, entitiesTOentity(opId, state.entities.power_slots)))
			return v;
	
	//Avoid Spells
	if (Bro->L->AvoidArea)
	{
		//Find New Stuff
		FindAvoidArea();

		//Avoid Stuff
		if (vAvoid.size() > 0)for (auto vv : MoveUnitsAway())v.push_back(vv);

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
			for (auto vv : sWaitForOP())v.push_back(vv);
			break;
			//Get Unit Close to OP Well/Orb
		case GetUnit:
			for (auto vv : sGetUnit())v.push_back(vv);
			break;
			//Get a Well
		case BuildWell:
			for (auto vv : sBuildWell())v.push_back(vv);
			break;
		case SpamBotX:
			for (auto vv : sSpamBotX())v.push_back(vv);
			break;
		case Fight:
			for (auto vv : sFight())v.push_back(vv);
			break;
		case PanicDef:
			for (auto vv : sPanicDef())v.push_back(vv);
			break;
		case DisablePanicDef:
			for (auto vv : sDisablePanicDef())v.push_back(vv);
			break;
		case Tier2:
			for (auto vv : sTier2())v.push_back(vv);
			break;
		case Tier1:
			for (auto vv : sTier1())v.push_back(vv);
			break;
		case DefaultDef:
			for (auto vv : sDefaultDef())v.push_back(vv);
			break;
	}


	/////////////Cleanups////////////////
	if (Bro->L->AvoidArea)if (vAvoid.size() > 0)RemoveFromMIS_AvoidArea(state.current_tick);
	if (state.current_tick % 100 == 1)RemoveFromSaveUnit();
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

bool FireBot::Stage()
{
	MISS;

	if (bStage && Bro->L->vStrategy.size() >0)
	{
		MISD("Old Stage: " + std::to_string(eStage) + "(" + std::to_string(iStageValue) + ") " + SwitchStagesText(eStage));
		eStage = (Stages)Bro->L->vStrategy[0].first;
		iStageValue = Bro->L->vStrategy[0].second;
		bStage = false;
		Bro->L->vStrategy.erase(Bro->L->vStrategy.begin());
		MISD("New Stage: " + std::to_string(eStage) + "(" + std::to_string(iStageValue) + ") " + SwitchStagesText(eStage));
		if(Bro->L->vStrategy.size() == 0)Bro->L->vStrategy.insert(Bro->L->vStrategy.begin(), std::make_pair(Fight, 0));
		return true;
	}

	///////////////////////////////////////////////
	/// Panic Stage based on staging <ANY> to 1 ///
	///////////////////////////////////////////////

	//Panic Def when mig army atck mmain base
	std::vector<capi::Entity> vMY_Tokens = entitiesTOentity(myId, lState.entities.token_slots);
	if (
		mapinfo.map == 1003 && // only for event
		iPanicDefCheck >= 0 &&
		vMY_Tokens.size() >0)
		if(	Bro->U->pointsInRadius(entitiesTOentity(opId, lState.entities.squads),
				capi::to2D(vMY_Tokens[0].position),
				100).size() >= 3
		&&	Bro->U->pointsInRadius(entitiesTOentity(myId, lState.entities.squads),
				capi::to2D(vMY_Tokens[0].position),
				100).size() <= 1
		)ChangeStrategy(PanicDef, 0);

	//OP builds wall
	if (entitiesTOentity(opId, lState.entities.barrier_modules).size() > 1 && 
		(int)lState.players[imyPlayerIDX].orbs.all == 1)
	{
		ChangeStrategy(Tier2, 0); // Def for 125 energy
		ChangeStrategy(DefaultDef, 125); // Def for 125 energy		
	}

	//I lost my Orb !!!!
	if ((int)lState.players[imyPlayerIDX].orbs.all == 0 && lState.current_tick > 1000)
	{
		ChangeStrategy(Tier1, 0);
	}

	//Good moment for Tier up
	// Total Power NOW + Power in Units (+ VOID * 0,5?) -> ME VS OP
	// My total Power Pool > X
	// Current Tick > Y

	//When Take another Power well??? (Max 6? or 40% wells of the map?)
		
		
	///////////////////////////////////////////////////////
	/// Default Stage based on currend Stage 1 to <ANY> ///
	///////////////////////////////////////////////////////
	switch (eStage)
	{
	case WaitForOP:
		if (lState.current_tick >= 100)ChangeStrategy(GetUnit, 0);
		if (entitiesTOentity(myId, lState.entities.squads).size() > 0)bStage = true;
		break;
	
	case SavePower:
	case DefaultDef:
		if ((int)lState.players[imyPlayerIDX].power > iStageValue)bStage = true;
		break;
		
	case PanicDef:
		if(vMY_Tokens.size() > 0)if (Bro->U->pointsInRadius(entitiesTOentity(opId, lState.entities.squads),
			capi::to2D(vMY_Tokens[0].position),
			100).size() == 0)ChangeStrategy(DisablePanicDef, 0);
		if (iPanicDefCheck >= 10)
		{
			iPanicDefCheck = -600;
			//ChangeStrategy(Fight, 0);
			bStage = true;
			MISD("Disable PanicDef");
		}
			break;
	case GetUnit: 
		if (entitiesTOentity(myId, lState.entities.squads).size() > 0)bStage = true;
		break;
		
	case DisablePanicDef:
		if(entitiesTOentity(myId, lState.entities.barrier_sets).size() == 0)bStage = true;
		break;
	case Tier2:
		if((int)lState.players[imyPlayerIDX].orbs.all >= 2)bStage = true;
		if (iTierReady == 0) // if orb is not build after 400 -> stage
		{
			iTierReady--;
			bStage = true;
		}
		break;
	case Tier1:
		if ((int)lState.players[imyPlayerIDX].orbs.all >= 1)bStage = true;
		if (iTierReady == 0) // if orb is not build after 400 -> stage
		{
			iTierReady--;
			bStage = true;
		}
		break;
	case BuildWell:
		if (iMyWells != -1 && iMyWells != entitiesTOentity(myId, lState.entities.power_slots).size())
		{
			iMyWells = -1;
			bStage = true;
		}
		break;
	case SpamBotX: 
	case Fight: 
		if (entitiesTOentity(myId, lState.entities.squads).size() == 0)ChangeStrategy(DefaultDef, 150);
		for (auto B : entitiesTOentity(myId, lState.entities.power_slots, lState.entities.token_slots))
			if(Bro->U->SquadsInRadius(opId, lState.entities.squads, capi::to2D(B.position), 25).size() > 0)
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
std::vector<capi::Command> FireBot::sWaitForOP()
{
	MISS;
	auto vReturn = std::vector<capi::Command>();
	if (lState.entities.squads.size() == 0)return vReturn;
	CalGlobalBattleTable(lState);

	vReturn.push_back(MIS_CommandProduceSquad(CardPickerFromBT(opBT, Swift, (int)lState.players[imyPlayerIDX].orbs.all),
		capi::to2D(entitiesTOentity(myId, lState.entities.token_slots)[0].position))); // Index 0 OK becaus opener

	iSkipTick = 50; // Wait till spwn is done

	MISE;
}

std::vector<capi::Command> FireBot::sBuildWell()
{
	MISS;

	auto vReturn = std::vector<capi::Command>();
	
	if (iMyWells == -1)	iMyWells = entitiesTOentity(myId, lState.entities.power_slots).size();

	// We need a Unit to go to the well
	if (entitiesTOentity(myId, lState.entities.squads).size() == 0)
	{
		ChangeStrategy(GetUnit, 0);
		return vReturn;
	}

	capi::Entity A;
	capi::Entity B;
	float fDistanc = 0;

	fDistanc = Bro->U->CloseCombi(entitiesTOentity(myId, lState.entities.squads),
		entitiesTOentity(0, lState.entities.power_slots), A, B);

	if (A.player_entity_id == myId)
	{
		vReturn.push_back(MIS_CommandGroupGoto({ A.id }, capi::to2D(B.position), capi::WalkMode_Normal));
		if (fDistanc < CastRange && lState.players[imyPlayerIDX].power >= 100)
			vReturn.push_back(capi::Command(MIS_CommandPowerSlotBuild(B.id)));			
	}
	MISE;
	return vReturn;
}

std::vector<capi::Command> FireBot::sGetUnit()
{
	MISS;
	auto vReturn = std::vector<capi::Command>();
	int iCard = CardPickerFromBT(opBT, None, (int)lState.players[imyPlayerIDX].orbs.all);

	if (SMJDeck[iCard].powerCost > lState.players[imyPlayerIDX].power)
	{
		MISEA("No Power: " + std::to_string(SMJDeck[iCard].powerCost) + ">" + std::to_string(lState.players[imyPlayerIDX].power));
		return vReturn;
	}

	capi::Entity A;
	capi::Entity B;

	Bro->U->CloseCombi(
		entitiesTOentity(myId, lState.entities.power_slots, lState.entities.token_slots, lState.entities.squads, lState.entities.buildings),
		entitiesTOentity(opId, lState.entities.power_slots, lState.entities.token_slots, lState.entities.squads),
		A, B);

	vReturn.push_back(MIS_CommandProduceSquad(iCard,
		Bro->U->Offseter(capi::to2D(A.position), capi::to2D(B.position), CastRange)));

	iSkipTick = 50; // Wait till spwn is done

	MISE;
}

std::vector<capi::Command> FireBot::sSpamBotX()
{
	MISS;

	auto vReturn = std::vector<capi::Command>();

	capi::Entity A;
	capi::Entity B;	
	int iCard = CardPickerFromBT(opBT, None, (int)lState.players[imyPlayerIDX].orbs.all);

	float fDistanc = Bro->U->CloseCombi(entitiesTOentity(myId, lState.entities.squads, lState.entities.buildings, lState.entities.power_slots, lState.entities.token_slots),
		entitiesTOentity(opId, lState.entities.power_slots, lState.entities.token_slots), A, B);

	if (SMJDeck[iCard].powerCost > lState.players[imyPlayerIDX].power)
	{
		//Cant Spawn insice the base (collition)
		if (fDistanc > CastRange * 2) fDistanc = CastRange;
		else fDistanc = CastRange * -1;

		vReturn.push_back(MIS_CommandProduceSquad(iCard,
			Bro->U->Offseter(capi::to2D(A.position), capi::to2D(B.position), fDistanc)));
	}

	for (auto E : entitiesTOentity(myId, lState.entities.squads))	
		if (E.job.variant_case == capi::JobCase::Idle)		
			vReturn.push_back(MIS_CommandGroupGoto({ E.id }, capi::to2D(B.position), capi::WalkMode_Normal));

	MISE;
	return vReturn;
}

std::vector<capi::Command> FireBot::sFight()
{
	MISS;
	auto vReturn = std::vector<capi::Command>();

	std::vector<capi::Squad> mySquat = Bro->U->FilterSquad(myId, lState.entities.squads);

	BattleTable myBT_Area;
	BattleTable opBT_Area;
	std::vector<int> PowerLevel;

	capi::Entity A;
	capi::Entity B;
	float fDistanc = Bro->U->CloseCombi(entitiesTOentity(myId, lState.entities.squads),
		entitiesTOentity(opId, lState.entities.power_slots, lState.entities.token_slots, lState.entities.squads, lState.entities.buildings, lState.entities.barrier_modules), A, B);

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
				for (auto F : lState.entities.figures)
					if(F.squad_id == S.entity.id)
						for (auto Asp : F.entity.aspects)
							if (Asp.variant_case == capi::AspectCase::Health)
								if (Asp.variant_union.health.current_hp / Asp.variant_union.health.cap_current_max < 0.3)
									{
										vSaveUnit.push_back(S.entity.id);
										Bro->U->CloseCombi({ S.entity },
											entitiesTOentity(opId, lState.entities.power_slots, lState.entities.token_slots, lState.entities.squads), A, B);
										vReturn.push_back(MIS_CommandGroupGoto({ S.entity.id },
											Bro->U->Offseter(capi::to2D(A.position), capi::to2D(B.position), CastRange * -1), capi::WalkMode_Force));
									}

			

			myBT_Area = CalcBattleTable(Bro->U->SquadsInRadius(myId, lState.entities.squads, capi::to2D(S.entity.position), FightRange));
			opBT_Area = CalcBattleTable(Bro->U->SquadsInRadius(opId, lState.entities.squads, capi::to2D(S.entity.position), FightRange));
			
			// if we are behind -> go to def mode
			MoreUnitsNeeded(myBT_Area, opBT_Area, PowerLevel);
			if(std::accumulate(PowerLevel.begin(), PowerLevel.end(), 0) < -1250)ChangeStrategy(DefaultDef, 150);

				if (lState.current_tick % 5 == 0 || NextCardSpawn == -1)NextCardSpawn = CardPickerFromBT(opBT_Area, None, (int)lState.players[imyPlayerIDX].orbs.all);

			if (SMJDeck[NextCardSpawn].powerCost < lState.players[imyPlayerIDX].power)
			{
				capi::Position2D SpawnPos;

				//if well near by go there
				std::vector<capi::Entity> UnDazed = Bro->U->pointsInRadius(entitiesTOentity(myId, lState.entities.power_slots, lState.entities.token_slots), capi::to2D(S.entity.position), 25);
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
						entitiesTOentity(opId, Bro->U->SquadsInRadius(opId, lState.entities.squads, capi::to2D(S.entity.position), FightRange)),
						A, B);
					SpawnPos = Bro->U->Offseter(capi::to2D(B.position), capi::to2D(A.position), CastRange); // ArcherRange);
				}
				//if melee go near
				else
				{
					MISD("Spawn Near");
					fDistanc = Bro->U->CloseCombi(entitiesTOentity(myId, Bro->U->SquadsInRadius(myId, lState.entities.squads, capi::to2D(S.entity.position), 75)),
						entitiesTOentity(opId, Bro->U->SquadsInRadius(opId, lState.entities.squads, capi::to2D(S.entity.position), FightRange)), A, B);
					if (fDistanc > CastRange)fDistanc = CastRange;
					SpawnPos = Bro->U->Offseter(capi::to2D(A.position), capi::to2D(B.position), fDistanc - 1, -1); //a bit offset
				}
				vReturn.push_back(MIS_CommandProduceSquad(NextCardSpawn, SpawnPos));
			}
		}


		for (auto vv : IdleToFight())vReturn.push_back(vv);
	}
	MISE;
	return vReturn;
}

std::vector<capi::Command> FireBot::sPanicDef()
{
	MISS;
	//bool OnWall;
	auto vReturn = std::vector<capi::Command>();
	//capi::Entity myOrb = entitiesTOentity(myId, lState.entities.token_slots)[0];
	std::vector<capi::Entity> myOrb = entitiesTOentity(myId, lState.entities.token_slots);
	if (myOrb.size() == 0) return vReturn;
	
	
	if (iWallReady == -1)
	{
		//Find My Wall
		std::vector<capi::Entity> barrier =
			Bro->U->pointsInRadius(entitiesTOentity(0, lState.entities.barrier_sets),
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
	unsigned int iArchers = CardPicker(99, 99, Archer, (int)lState.players[imyPlayerIDX].orbs.all);
	if (lState.players[imyPlayerIDX].power > SMJDeck[iArchers].powerCost)
	{
		vReturn.push_back(MIS_CommandProduceSquad(iArchers, capi::to2D(myOrb[0].position)));
	}

	//Archers on the Wall
	
	std::vector < capi::Entity> myBarrier = entitiesTOentity(myId, lState.entities.barrier_modules);
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


	
	for (auto S : Bro->U->pointsInRadius(entitiesTOentity(myId, lState.entities.squads), capi::to2D(myOrb[0].position), 50))
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
	for (auto E : entitiesTOentity(myId, lState.entities.squads))
		if(Bro->U->distance(capi::to2D(E.position), capi::to2D(GatePos))>25)
		if (E.job.variant_case == capi::JobCase::Idle)
			vReturn.push_back(MIS_CommandGroupGoto({ E.id }, capi::to2D(GatePos), capi::WalkMode_Normal));
	
	MISE;
	return vReturn;
}

std::vector<capi::Command> FireBot::sDisablePanicDef()
{
	MISS;
	
	auto vReturn = std::vector<capi::Command>();
	for (auto B : entitiesTOentity(myId, lState.entities.barrier_sets))
		vReturn.push_back(MIS_CommandGroupKillEntity({ B.id }));

	iWallReady = 70 * 10; //ready in ~65 sec
	iPanicDefCheck = 0;

	MISE;
	return vReturn;
}

std::vector<capi::Command> FireBot::sTier2()
{
	MISS;

	auto vReturn = std::vector<capi::Command>();

	//Tier 2 good idea? or wait -> Def + 50 Power
	//Battle Tabel + Power (+ Void)

	//find T2 Spot

	//Build T2 if still good idea

	//check if orb is in cosntruction, if yes STAGE

	iTierReady = 400;

	bStage = true;

	MISE;
	return vReturn;
}

std::vector<capi::Command> FireBot::sDefaultDef()
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

	auto eBase = entitiesTOentity(myId, lState.entities.power_slots, lState.entities.token_slots);

	//If units fare between 50 and 100 Pull bakc
	// >100 will be send on a atack later
	for (auto eSquat : entitiesTOentity(myId, lState.entities.squads))
	{
		fDisntanc = Bro->U->CloseCombi({ eSquat }, eBase, A, B);
		if (fDisntanc > 50 && fDisntanc < 100)
			vReturn.push_back(MIS_CommandGroupGoto({ A.id }, capi::to2D(B.position), capi::WalkMode_Normal));
	}
	
	for (auto EE : eBase)
	{
		//when no OP is close the get all units in range to get closer + NEXT
		opUnits = Bro->U->SquadsInRadius(opId, lState.entities.squads, capi::to2D(EE.position), 50);
		myUnits = Bro->U->SquadsInRadius(myId, lState.entities.squads, capi::to2D(EE.position), 50);
		if (opUnits.size() == 0)
		{
			for (auto EEE : myUnits)
				if(Bro->U->CloseCombi({ EEE.entity }, eBase, A, B) > HealRange)
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
				NextCardSpawn = CardPickerFromBT(opBT_Area, None, (int)lState.players[imyPlayerIDX].orbs.all);
				if(SMJDeck[NextCardSpawn].powerCost < lState.players[imyPlayerIDX].power)
					vReturn.push_back(
					MIS_CommandProduceSquad(NextCardSpawn,capi::to2D(EE.position)));
			}
		}
	}

	//Ideal Units do stuff
	for (auto vv : IdleToFight())vReturn.push_back(vv);
				
	MISE;
	return vReturn;
}

std::vector<capi::Command> FireBot::sTier1()
{
	MISS;

	auto vReturn = std::vector<capi::Command>();

	//Where did i lost it?
	//Can i kill OP units there and rebuild?
	//-> Yes DO IT
	//-> No run away with one unit(SWITF?) (Top 3 closest, but most fare awai from OP Enteties)
	//Rebuild and def til ???

	MISE;
	return vReturn;
}
