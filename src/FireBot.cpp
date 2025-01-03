﻿#define DF_Debug

#include "../incl/FireBot.h"
#include "../incl/CardBaseSMJ.h"
#include "../incl/Util.h"
#include "../incl/LOAD.h"

#include "../incl/FireBot_AvoidArea.h"
#include "../incl/FireBot_RejectedComamand.h"
#include "../incl/FireBot_BattleTable.h"
#include "../incl/FireBot_CardPicker.h"
#include "../incl/FireBot_Util.h"

broker* (FireBot::Bro) = NULL;

bool comparePower(BaseDef* a, BaseDef* b)
{
	return a->PowerLevelSum() < b->PowerLevelSum();
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

void FireBot::PrepareForBattle(const capi::MapInfo& mapInfo, const capi::Deck& deck)
{
	MISS;
	MISD("PrepareForBattle: " + std::to_string(mapInfo.map) + " with " + deck.name);
	mapinfo = mapInfo;
	
	MaxAvoidID = 0;
	NextCardSpawn = -1;
	iWallReadyTick = 0;
	iMyWells = -1;
	//iMyOrbs = -1;
	iPanicDefCheck = 0;
	TierCheckTick = Bro->L->Tier2Init;
	//bTier2VSWall = true;
	opUsingWalls = false;
	iSkipTick = 0;
	WellCheckTick = 0; // 100

//if mapInfo.map == 1003 -> no Panic Def
	eStage = WaitForOP;
	eStageNext = NoStage;
	iStageValue = 0;
	iStageValueNext = 0;
	TierReadyTick = 0;

	vAvoid.clear();
	RejectedComamandChecklist.clear();
	vIdler.clear();
	vSaveUnit.clear();
	SMJDeck.clear();
	vBases.clear();
	for (auto apiCard : deck.cards)	SMJDeck.push_back(Bro->J->CardFromJson(apiCard % 1000000));
#ifdef MIS_Stream 
	Bro->L_GamesPlus();
#endif

	MISE;
}

std::vector<capi::Deck> FireBot::DecksForMap(const capi::MapInfo& mapInfo)
{
	MISS;	
	
	MISD("DecksForMap: " + mapInfo.map);
	MISD("Name: " + Name);
	unsigned int i = 0;
	auto v = std::vector<capi::Deck>();
	auto deck = capi::Deck();


	if (Name == "FireDeck" || Name == "FireBot")
	{
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
	}

	if (Name == "ShadowDeck" || Name == "FireBot")
	{
		deck.name = "ShadowDeck";
		i = 0;
		deck.cover_card_index = 19;
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Dreadcharger, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Forsaken, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::NoxTrooper, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Motivate, capi::Upgrade_U3);

		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::NastySurprise, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::LifeWeaving, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Executor, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::DarkelfAssassins, capi::Upgrade_U3);

		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Nightcrawler, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::ShadowMage, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::KnightOfChaosAFrost, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::CorpseExplosion, capi::Upgrade_U3);

		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::NetherWarpAFrost, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Harvester, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::AuraofCorruption, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::CultistMaster, capi::Upgrade_U3);

		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::AshbonePyro, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::SatanaelAFire, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::BloodHealing, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Voidstorm, capi::Upgrade_U3);

		v.push_back(deck);
	}

	if (Name == "NatureDeck" || Name == "FireBot")
	{
		deck.name = "NatureDeck";
		i = 0;
		deck.cover_card_index = 19;
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Swiftclaw, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::DryadAFrost, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Windweavers, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Shaman, capi::Upgrade_U3);

		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Spearmen, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::EnsnaringRoots, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Hurricane, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::SurgeOfLight, capi::Upgrade_U3);

		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Burrower, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Ghostspears, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::SpiritHuntersANature, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::DeepOneANature, capi::Upgrade_U3);

		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::EnergyParasite, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::CurseofOink, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::CreepingParalysis, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Parasite, capi::Upgrade_U3);

		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::ParasiteSwarm, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Thunderstorm, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::FathomLord, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Mo, capi::Upgrade_U3);

		v.push_back(deck);
	}
	if (Name == "FrostDeck" || Name == "FireBot")
	{
		deck.name = "FrostDeck";
		i = 0;
		deck.cover_card_index = 19;
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::MasterArchers, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::IceGuardian, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::LightbladeAShadow, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::FrostBiteAShadow, capi::Upgrade_U3);

		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::GlyphofFrost, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::IceBarrier, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::HomeSoil, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::GlacierShell, capi::Upgrade_U3);

		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::KoboldTrick, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::IcefangRaptorAFrost, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::MountainRowdyAShadow, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::StormsingerANature, capi::Upgrade_U3);

		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::WhiteRangers, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::WarEagle, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Coldsnap, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::AreaIceShield, capi::Upgrade_U3);

		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::TimelessOne, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::SilverwindLancers, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Tremor, capi::Upgrade_U3);
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::ShieldBuilding, capi::Upgrade_U3);

		v.push_back(deck);
	}

	MISE;
	return v;
}

void FireBot::MatchStart(const capi::GameStartState& state)
{
	MISS;
	
	MISERROR(("MatchStart at " + std::to_string(Bro->L_getEEE_Now())).c_str());
	
	if (Bro->L->DistancTypeTwo)Bro->U->InitWalls(mapinfo.map);
	
	//First Get My ID and Index
	for (int iPlayerCount = 0; iPlayerCount < state.players.size(); iPlayerCount++)
		if (state.players[iPlayerCount].entity.id == state.your_player_id)
		{
			myId = state.players[iPlayerCount].entity.id;
			imyPlayerIDX = iPlayerCount;
		}

	//Sec. Get First OP ID and Index
	for (int iPlayerCount = 0; iPlayerCount < state.players.size(); iPlayerCount++)
		if (state.players[iPlayerCount].entity.id != state.your_player_id && 
			state.players[iPlayerCount].entity.power > 0 &&
			state.players[iPlayerCount].entity.team != state.players[imyPlayerIDX].entity.team)
		{
			opId = state.players[iPlayerCount].entity.id;
			iopPlayerIDX = iPlayerCount;
		}
	
	MISD("MY ID: " + std::to_string(myId));
	MISD("OP ID: " + std::to_string(opId));	

	EruptionPos = -1;
	iArchers = -1;
	for (unsigned int i = 0; i < SMJDeck.size(); i++)
	{
		if (SMJDeck[i].cardName == "Eruption" && EruptionPos==-1)EruptionPos = i;
		if (SMJDeck[i].unitClass == "Archer"  && iArchers==-1)iArchers = i;
	}
	if (EruptionPos == -1)
	{
		MISD("No Eruption in deck");		
		Bro->L->UnitEruption = false;
	}
	
	eMainOrb = entitiesTOentity(myId, state.entities.token_slots)[0];
	MISE;
}


std::vector<capi::Command> FireBot::Tick(const capi::GameState& state)
{		
	lState = state;
	auto v = std::vector<capi::Command>();
	auto vTemp = std::vector<capi::Command>();

#ifdef MIS_DEBUG

	if (Bro->L->AllTick && state.current_tick % 10 == 1)
	{
		//MISD(std::to_string(eStage) + "#" + Bro->sTime(state.current_tick) + "#" + std::to_string(state.current_tick) + "#" + std::to_string(state.players[imyPlayerIDX].power));
		std::vector<int> plTemp;
		MoreUnitsNeeded(myBT, opBT, plTemp);
		MISD(std::to_string(plTemp[0]) + " # " + std::to_string(plTemp[1]) + " # " + std::to_string(plTemp[2]) + " # " + std::to_string(plTemp[3]) + " # " + std::to_string(state.players[imyPlayerIDX].power));
	}

	if (Bro->sComand != "")
	{
		if (Bro->sComand == "?")
		{
			MISERROR("-----------------------------------");
			MISERROR("# dump          = Save tick        ");
			MISERROR("# dumpx         = Save Commands    ");			
			MISERROR("# bt            = Print BattleTable");
			MISERROR("# st            = echo strategy    ");
			MISERROR("# pl            = echo power level ");
			MISERROR("# i             = Bot Infos        ");
			MISERROR("# R;[ID];[Rang] = Draw Radius      ");
			MISERROR("-----------------------------------");
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

		if (Bro->sComand == "pl")
		{
			std::vector<int> plTemp;
			MoreUnitsNeeded(myBT, opBT, plTemp);
			for (auto i : plTemp)MISD(i);
			Bro->sComand = "";
		}

		if (Bro->L->entry(Bro->sComand,0) == "R")
		{
			MISD(Bro->L->entry(Bro->sComand, 0));
			MISD(std::atoi(Bro->L->entry(Bro->sComand, 1).c_str()));
			MISD(Bro->L->entry(Bro->sComand, 2));
			for (auto E : entitiesTOentity(myId, lState.entities.ability_world_objects, lState.entities.barrier_modules, lState.entities.barrier_sets, lState.entities.buildings, lState.entities.figures, lState.entities.power_slots, lState.entities.squads, lState.entities.token_slots))
				if (E.id == std::atoi(Bro->L->entry(Bro->sComand, 1).c_str()))
					for (auto vvv : Bro->U->DrawCircle(capi::to2D(E.position), std::atoi(Bro->L->entry(Bro->sComand, 2).c_str())))
						v.push_back(vvv);
			for (auto E : entitiesTOentity(opId, lState.entities.ability_world_objects, lState.entities.barrier_modules, lState.entities.barrier_sets, lState.entities.buildings, lState.entities.figures, lState.entities.power_slots, lState.entities.squads, lState.entities.token_slots))
				if (E.id == std::atoi(Bro->L->entry(Bro->sComand, 1).c_str()))
					for (auto vvv : Bro->U->DrawCircle(capi::to2D(E.position), std::atoi(Bro->L->entry(Bro->sComand, 2).c_str())))
						v.push_back(vvv);
			for (auto E : entitiesTOentity(0, lState.entities.ability_world_objects, lState.entities.barrier_modules, lState.entities.barrier_sets, lState.entities.buildings, lState.entities.figures, lState.entities.power_slots, lState.entities.squads, lState.entities.token_slots))
				if (E.id == std::atoi(Bro->L->entry(Bro->sComand, 1).c_str()))
					for (auto vvv : Bro->U->DrawCircle(capi::to2D(E.position), std::atoi(Bro->L->entry(Bro->sComand, 2).c_str())))
						v.push_back(vvv);
			Bro->sComand = "";
		}

		if (Bro->sComand == "i")
		{
			MISD("current_tick: " + std::to_string(state.current_tick));
			MISD("Power:        " + std::to_string(lState.players[imyPlayerIDX].power));
			MISD("WellCheckTick:" + std::to_string(WellCheckTick));
			MISD("TierReadyTick:" + std::to_string(TierReadyTick));
			MISD("TierCheckTick:" + std::to_string(TierCheckTick));
			MISD("BuildWellOrbC:" + std::to_string(BuildWellOrbCheck()));

			MISD("eStage       :" + SwitchStagesText(eStage));
			MISD("iStageValue  :" + std::to_string(iStageValue));
			
			Bro->sComand = "";
		}


		//Bro->sComand = "";
	}
#endif

	for (auto r : state.rejected_commands)
	{
#ifdef MIS_DEBUG
		if (state.current_tick % 50 == 0 && Bro->L->DrawAvoidArea)
			for (auto vv : vAvoid)
				for (auto vvv : Bro->U->DrawCircle(vv->pos, vv->radius))
					v.push_back(vvv);

		if (Bro->L->EchoRejected)
		{
			MISD("rejected Player: " + std::to_string(r.player));
			MISD("reason         : " + Bro->U->switchCommandRejectionReason(r.reason));
			MISD("command        : " + Bro->U->switchCommand(r.command));
		}
#endif // MIS_DEBUG	
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
	
	if (iPanicDefCheck < 0)iPanicDefCheck++;

	//WELL KILLER
	if (Bro->L->WellKiller && EruptionPos != -1 && state.current_tick % Bro->L->WellKillerInterval == 1)
		for (auto vv : WellKiller())v.push_back(vv);
	if (v.size() > 0) return v;
	
	if (Bro->L->UnitEruption && EruptionPos != -1 && state.current_tick % Bro->L->UnitEruptionInterval == 1)
		for (auto vv : CoolEruption())v.push_back(vv);

	if (Bro->L->InstantRepair && state.current_tick % Bro->L->InstaRepairInterval == 1)
		for (auto vv : InstantRepairFunction())v.push_back(vv);
	
	if (Bro->L->BattleTable && state.current_tick % Bro->L->BattleTableInterval == 1)
		CalGlobalBattleTable();

	if (state.current_tick % Bro->L->UpdateBasesInterval == 1)
		UpdateBases();


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
	

	////////////////Strategy ////////////////////
	
	if (state.current_tick % Bro->L->StrategyInterval == 1)
	{
		if (CalcStrategy(state))
		{
			SwitchStrategy();
			if (Bro->L->ResetUnitsOnStratSwitch)for (auto vv : ResetUnits())v.push_back(vv);
		}
	}
		
	
	if (iSkipTick < state.current_tick) switch (eStage)
	{
		case GetUnit:
			for (auto vv : sGetUnit())v.push_back(vv);
			break;
			//Get a Well
		case BuildWell:
			for (auto vv : sBuildWell())v.push_back(vv);
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
		case TierUp:
			for (auto vv : sTierUp())v.push_back(vv);
			break;		
		case DefaultDef:
		case WaitTier:
			for (auto vv : sDefaultDef())v.push_back(vv);
			break;
	}
	

	/////////////Cleanups////////////////
	if (Bro->L->AvoidArea)if (vAvoid.size() > 0)RemoveFromMIS_AvoidArea(state.current_tick);
	if (state.current_tick % Bro->L->CleanSaveUnitInterval == 1)RemoveFromSaveUnit();
	if (state.current_tick % Bro->L->FixType2Interval == 1)for (auto vv : FixGroupGotoType2())v.push_back(vv);
	CleanUpRejectedComamandChecklist();

	//Remove dobbel orders

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


void FireBot::SwitchStrategy()
{
	MISS;
	//MISERROR("Switch: " + std::to_string(eStage) + " -> " + std::to_string(eStageNext));
	//MISERROR("Switch: " + SwitchStagesText(eStage) + " -> " + SwitchStagesText(eStageNext));
	//MISERROR("Switch: " + std::to_string(iStageValue) + " -> " + std::to_string(iStageValueNext));
	MISERROR((std::to_string(lState.current_tick) + " Switch: " + SwitchStagesText(eStage) + " (" + std::to_string(iStageValue) + ") -> " + SwitchStagesText(eStageNext) + " ("  + std::to_string(iStageValueNext) + ")").c_str());
	eStage = eStageNext;
	iStageValue = iStageValueNext;
	MISE;
}

bool FireBot::CalcStrategy(const capi::GameState& StrategyState)
{
	MISS;

	///////////////////////////////////////////////
	/// Panic Stage based on staging <ANY> to 1 ///
	///////////////////////////////////////////////

	//Panic Def when mig army atck mmain base
	BattleTable myBT_Area;
	BattleTable opBT_Area;
	std::vector<int> PowerLevel;

	bool bReturn = false;

	if (eStage != PanicDef &&
		OrbOneOK() && // Is Main Orb is sill alive?
		mapinfo.map == 1003 && // only for event
		iPanicDefCheck >= 0 &&
		iWallReadyTick <= lState.current_tick) // Is Wall Ready?
		if (Bro->U->pointsInRadius(entitiesTOentity(opId, StrategyState.entities.squads),
			capi::to2D(eMainOrb.position),
			Bro->L->PanicDefSearch).size() >= 3
			&& Bro->U->pointsInRadius(entitiesTOentity(myId, StrategyState.entities.squads),
				capi::to2D(eMainOrb.position),
				Bro->L->PanicDefSearch).size() <= 1)
		{
			bReturn = SetNextStrategy(PanicDef, 0);
			MISEA("PRIO Return 1");
			return true;
		}

	//OP builds wall
	if (eStage == Fight &&
		opUsingWalls == false &&
		entitiesTOentity(opId, StrategyState.entities.barrier_modules).size() > 1 &&
		entitiesTOentity(myId, StrategyState.entities.token_slots).size() == 1)
	{
		bReturn = SetNextStrategy(TierUp, 99); // Def for 125 energy
		TierCheckTick = 0;
		Bro->L->TierCheckOffset = Bro->L->TierCheckOffset / 3; // CHeck for Tier two more often
		opUsingWalls = true;
		MISEA("PRIO Return 2");
		return true;
	}

	// Lost my Tier 1!!!
	if (eStage != TierUp && 
		entitiesTOentity(myId, StrategyState.entities.token_slots).size() < 1)
	{
		bReturn = SetNextStrategy(TierUp, 1);
		MISEA("PRIO Return 3");
		return true;
	}

	//A Base is under Atack.
	if (eStage != PanicDef && 
		eStage != DefaultDef &&
		!( eStage == TierUp && iStageValue == 1)) for (auto B : vBases)
	{
		if (B->Alart)bReturn = SetNextStrategy(DefaultDef, 159); // B->Base.id);
	}


	if (eStage != BuildWell && iMyWells != -1)iMyWells = -1;

	if (eStage != DisablePanicDef && eStage != PanicDef )
		if(entitiesTOentity(myId, StrategyState.entities.barrier_sets).size() > 0)
			bReturn = SetNextStrategy(DisablePanicDef, 0);

	

	
	//if (eStage != TierUp    && iMyOrbs  != -1)iMyOrbs  = -1;
	

	///////////////////////////////////////////////////////
	/// Default Stage based on currend Stage 1 to <ANY> ///
	///////////////////////////////////////////////////////
	switch (eStage)
	{
	case WaitForOP:
		if (StrategyState.current_tick >= 100)bReturn = SetNextStrategy(GetUnit, Bro->L->WaitSpawnTime);
		if (StrategyState.entities.squads.size() > 0)bReturn = SetNextStrategy(GetUnit, Bro->L->WaitSpawnTime);
		break;
	
	case SavePower:
	case DefaultDef:	

		//Base Def Mode
		unsigned int iCount;
		iCount = 0;
		for (auto B : vBases)if (B->PowerLevelSum() >= 0) iCount++; 

		if( iCount == vBases.size() &&
			(int)StrategyState.players[imyPlayerIDX].power > iStageValue) bReturn = SetNextStrategy(Fight, 10);

		/*

		if (iStageValue > 1000)
		{
			bool bFound = false;
			for (auto B : vBases)if (B->Base.id == iStageValue)
			{
				bFound = true;
				if (B->PowerLevelSum() >= 0) bReturn = SetNextStrategy(Fight, 10);
			}

			//Lost THe base
			if (!bFound)bReturn = SetNextStrategy(DefaultDef, 155);
		}

		if ((int)StrategyState.players[imyPlayerIDX].power > iStageValue)bReturn = SetNextStrategy(Fight, 2);
		*/
		break;
		
	case PanicDef:
		if (Bro->U->pointsInRadius(entitiesTOentity(opId, StrategyState.entities.squads),
			capi::to2D(eMainOrb.position),
			Bro->L->PanicDefSearch).size() == 0)bReturn = SetNextStrategy(DisablePanicDef, 0);
		/*
		if (iPanicDefCheck >= 3)
		{
			iPanicDefCheck = -600;
			bReturn = SetNextStrategy(DefaultDef, 52);
		}*/
			break;
	case GetUnit: 
		if (entitiesTOentity(myId, StrategyState.entities.squads).size() > 0)bReturn = SetNextStrategy(Fight, 1);
		break;
		
	case DisablePanicDef:
		if(entitiesTOentity(myId, StrategyState.entities.barrier_sets).size() == 0)bReturn = SetNextStrategy(Fight, 3);
		break;	
	case TierUp:
		if (entitiesTOentity(myId, StrategyState.entities.squads).size() == 0)bReturn = SetNextStrategy(GetUnit, Bro->L->WaitSpawnTime);
		for (auto O : entitiesTOentity(myId, StrategyState.entities.token_slots))if (O.job.variant_case == capi::JobCase::Construct)
		{
			TierReadyTick = StrategyState.current_tick + Bro->L->OrbBuildWait;
			bReturn = SetNextStrategy(WaitTier, entitiesTOentity(myId, StrategyState.entities.token_slots).size());
		}
		
		if (TierReadyTick > StrategyState.current_tick)bReturn = SetNextStrategy(WaitTier, iStageValue);
		if (TierCheckTick > StrategyState.current_tick)bReturn = SetNextStrategy(Fight, 6);
		if(BuildWellOrbCheck()== false && iStageValue != 99 && iStageValue != 1)bReturn = SetNextStrategy(Fight, 4);
		break;
	case WaitTier:
		if (TierReadyTick < StrategyState.current_tick)bReturn = SetNextStrategy(Fight, 5);
		//if (entitiesTOentity(myId, StrategyState.entities.token_slots).size() >= iStageValue || iStageValue == 99)bReturn = SetNextStrategy(Fight, 6);
		break;

	case BuildWell:
		if (entitiesTOentity(myId, StrategyState.entities.squads).size() == 0)bReturn = SetNextStrategy(GetUnit, Bro->L->WaitSpawnTime);
		if (WellCheckTick > StrategyState.current_tick)bReturn = SetNextStrategy(Fight, 7);
		if (iMyWells != -1 && iMyWells != entitiesTOentity(myId, StrategyState.entities.power_slots).size())
		{
			WellCheckTick = StrategyState.current_tick + Bro->L->WellCheckOffset;
			bReturn = SetNextStrategy(DefaultDef, 53);
		}
		//if (WellCheckTick > StrategyState.current_tick)bReturn = SetNextStrategy(Fight, 8);
		if(BuildWellOrbCheck() == false)bReturn = SetNextStrategy(DefaultDef, 54);
		break;	
	case Fight: 
		if (entitiesTOentity(myId, StrategyState.entities.squads).size() == 0)bReturn = SetNextStrategy(GetUnit, Bro->L->WaitSpawnTime);
		for (auto B : entitiesTOentity(myId, StrategyState.entities.power_slots, StrategyState.entities.token_slots))
			if(Bro->U->SquadsInRadius(opId, StrategyState.entities.squads, capi::to2D(B.position), Bro->L->SwitchToDefRange).size() > 0)
				bReturn = SetNextStrategy(DefaultDef, 75);

		if (BuildWellOrbCheck() )			
		{
			switch(entitiesTOentity(myId, StrategyState.entities.token_slots).size())
			{
				case 0:
				case 1:
					if (TierCheckTick < StrategyState.current_tick)bReturn = SetNextStrategy(TierUp, 2);
					else TierCheckTick = StrategyState.current_tick + Bro->L->TierCheckOffset;
					break;
				case 2 :
					if (Bro->L->Tier3Init < StrategyState.current_tick)bReturn = SetNextStrategy(TierUp, 3);
					break;
			}			
		}

		if (BuildWellOrbCheck() && 
				// Max 4 Wells
			(  entitiesTOentity(myId, StrategyState.entities.power_slots).size() < 4 
				//Stay + 1
			|| entitiesTOentity(myId, StrategyState.entities.power_slots).size() <= entitiesTOentity(opId, StrategyState.entities.power_slots).size())
			&& WellCheckTick < StrategyState.current_tick)bReturn = SetNextStrategy(BuildWell, 2);
		else 
		{
			//Earlyer CHekf if behind
			if(entitiesTOentity(myId, StrategyState.entities.power_slots).size() < entitiesTOentity(opId, StrategyState.entities.power_slots).size())
				WellCheckTick = StrategyState.current_tick + Bro->L->WellCheckOffset * 0.25;
			else WellCheckTick = StrategyState.current_tick + Bro->L->WellCheckOffset;
		}

		for (auto S : Bro->U->FilterSquad(myId, StrategyState.entities.squads))
			if (S.entity.job.variant_case == capi::JobCase::AttackSquad)
			{
				myBT_Area = CalcBattleTable(Bro->U->SquadsInRadius(myId, StrategyState.entities.squads, capi::to2D(S.entity.position), Bro->L->FightRange));
				opBT_Area = CalcBattleTable(Bro->U->SquadsInRadius(opId, StrategyState.entities.squads, capi::to2D(S.entity.position), Bro->L->FightRange));

				// if we are behind -> go to def mode
				MoreUnitsNeeded(myBT_Area, opBT_Area, PowerLevel);
				//MISD(std::accumulate(PowerLevel.begin(), PowerLevel.end(), 0));
				if (std::accumulate(PowerLevel.begin(), PowerLevel.end(), 0) < Bro->L->GiveUpFight)bReturn = SetNextStrategy(DefaultDef, 152);
			}

		break;
	}
	
	MISE;
	return bReturn;
}

std::vector<capi::Command> FireBot::sBuildWell()
{
	MISS;

	auto vReturn = std::vector<capi::Command>();
	
	if (iMyWells == -1)	iMyWells = entitiesTOentity(myId, lState.entities.power_slots).size();

	// We need a Unit to go to the well
	if (entitiesTOentity(myId, lState.entities.squads).size() == 0)
	{
		MISEA("Need Unit");
		return vReturn;
	}

	capi::Entity A;
	capi::Entity B;
	capi::Entity C;
	float fDistanc = 0;

	fDistanc = Bro->U->CloseCombi(entitiesTOentity(myId, lState.entities.squads),
		entitiesTOentity(0, lState.entities.power_slots), A, B);

	//No Wells to take
	if (fDistanc == 99999)
	{
		WellCheckTick = lState.current_tick + Bro->L->WellCheckOffset;
		return vReturn;
	}

	//Is OP near that WELL?
	if (Bro->U->CloseCombi(entitiesTOentity(opId, lState.entities.squads), { B }, C, B) < Bro->L->SaveRangeWellOrb)
	{
		WellCheckTick = lState.current_tick + Bro->L->WellCheckOffset;
		return vReturn;
	}

	if (A.player_entity_id == myId)
	{
		vReturn.push_back(MIS_CommandGroupGoto({ A.id }, capi::to2D(B.position), capi::WalkMode_Normal));
		if (fDistanc < Bro->L->CastRange && lState.players[imyPlayerIDX].power >= 100)
			vReturn.push_back(capi::Command(MIS_CommandPowerSlotBuild(B.id)));			
	}

	for (auto vv : IdleToFight())vReturn.push_back(vv);
	if (Bro->L->TragetSwitcher)for (auto vv : SwitchTargets())vReturn.push_back(vv);

	MISE;
	return vReturn;
}

std::vector<capi::Command> FireBot::sGetUnit()
{
	MISS;
	auto vReturn = std::vector<capi::Command>();
	BattleTable btTemp = CalcBattleTable(Bro->U->FilterSquad(opId, lState.entities.squads));
	int iCard = CardPickerFromBT(btTemp, None);

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

	MISD("iReturn=" + std::to_string(iCard) + "#" + SMJDeck[iCard].cardName);
	vReturn.push_back(MIS_CommandProduceSquad(iCard,
		Bro->U->Offseter(capi::to2D(A.position), capi::to2D(B.position), Bro->L->CastRange)));

	iSkipTick = lState.current_tick + iStageValue; // Wait till spwn is done

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

	//NO unit? get any unit
	if (mySquat.size() == 0)
	{
		//ChangeStrategy(GetUnit, 0);
		MISEA("No Units change Stage");
		return vReturn;
	}

	for (auto S : mySquat)
	{		
		float fDistanc = Bro->U->CloseCombi({ S.entity },
			entitiesTOentity(opId, lState.entities.power_slots, lState.entities.token_slots, lState.entities.squads, lState.entities.buildings, lState.entities.barrier_modules), A, B);

		if (fDistanc < Bro->L->FightRange) // Unit in Combet pr near
		{							
			//Fall Back with Archers
			if (CARD_ID_to_SMJ_CARD(S.card_id).attackType == 1)
				if (std::find(vSaveUnit.begin(), vSaveUnit.end(), S.entity.id) == vSaveUnit.end())
					if (GetSquadHP(S.entity.id) / GetSquadMAXHP(S.entity.id) < 0.3)
					{
						vSaveUnit.push_back(S.entity.id);
						Bro->U->CloseCombi({ S.entity },
							entitiesTOentity(opId, lState.entities.power_slots, lState.entities.token_slots, lState.entities.squads), A, B);
						vReturn.push_back(MIS_CommandGroupGoto({ S.entity.id },
							Bro->U->Offseter(capi::to2D(A.position), capi::to2D(B.position), Bro->L->CastRange * -1), capi::WalkMode_Force));
					}

			

			myBT_Area = CalcBattleTable(Bro->U->SquadsInRadius(myId, lState.entities.squads, capi::to2D(S.entity.position), Bro->L->FightRange));
			opBT_Area = CalcBattleTable(Bro->U->SquadsInRadius(opId, lState.entities.squads, capi::to2D(S.entity.position), Bro->L->FightRange));
			MoreUnitsNeeded(myBT_Area, opBT_Area, PowerLevel);
			
			if (entitiesTOentity(myId, lState.entities.token_slots).size() == 2 && TierReadyTick < lState.current_tick
				&&
				//With High Power Level and Tier to go Sieg
				(std::accumulate(PowerLevel.begin(), PowerLevel.end(), 0) > Bro->L->AddSiegeToMix
					//Or OP gos Wall
					||
					Bro->U->EntitiesInRadius(opId, entitiesTOentity(opId, lState.entities.barrier_modules), capi::to2D(S.entity.position), 500).size() > 0)
					&&
					std::accumulate(PowerLevel.begin(), PowerLevel.end(), 0) > Bro->L->AddSiegeToMix / 2
				)
			{
				NextCardSpawn = CardPickerFromBT(opBT_Area, Siege);
			}
			else NextCardSpawn = CardPickerFromBT(opBT_Area, None);

			if (SMJDeck[NextCardSpawn].powerCost < lState.players[imyPlayerIDX].power)
			{
				capi::Position2D SpawnPos;

				//if well near by go there
				std::vector<capi::Entity> UnDazed = Bro->U->pointsInRadius(entitiesTOentity(myId, lState.entities.power_slots, lState.entities.token_slots), capi::to2D(S.entity.position), 25);
				if (UnDazed.size() > 0)
				{
					//MISD("Spawn undazed");
					SpawnPos = Bro->U->Offseter(capi::to2D(UnDazed[0].position), capi::to2D(S.entity.position), Bro->L->CastRange);

				}
				//if archer go away
				else if (SMJDeck[NextCardSpawn].attackType == 1) //ranged
				{
					//MISD("Spawn Range Distance");
					Bro->U->CloseCombi({ S.entity },
						entitiesTOentity(opId, Bro->U->SquadsInRadius(opId, lState.entities.squads, capi::to2D(S.entity.position), Bro->L->FightRange)),
						A, B);
					SpawnPos = Bro->U->Offseter(capi::to2D(B.position), capi::to2D(A.position), Bro->L->CastRange); // ArcherRange);
				}
				//if melee go near
				else
				{
					//MISD("Spawn Near");
					auto opSquads = entitiesTOentity(opId, Bro->U->SquadsInRadius(opId, lState.entities.squads, capi::to2D(S.entity.position), Bro->L->FightRange));
					if(opSquads.size() > 0) fDistanc = Bro->U->CloseCombi(entitiesTOentity(myId, Bro->U->SquadsInRadius(myId, lState.entities.squads, capi::to2D(S.entity.position), 75)), opSquads, A, B);
					if (fDistanc > Bro->L->CastRange)fDistanc = Bro->L->CastRange;
					SpawnPos = Bro->U->Offseter(capi::to2D(A.position), capi::to2D(B.position), fDistanc - 1, -1); //a bit offset
				}
				vReturn.push_back(MIS_CommandProduceSquad(NextCardSpawn, SpawnPos));
#ifdef MIS_DEBUG
				if (Bro->L->DrawSpawn)vReturn.push_back(MIS_Ping_Attention(SpawnPos));
#endif // MIS_DEBUG	
			}
		
			/*
			* !!!!!!!!!!!!!!!!!!!!!
			//Stop current units (IdleToFight will find new target)
			if (S.entity.job.variant_case == capi::JobCase::Goto)
				if(S.entity.job.variant_union.gotoCppField.walk_mode == capi::WalkMode_Normal)
				{
					vReturn.push_back(MIS_CommandGroupStopJob({ S.entity.id }));
					vSaveUnit.push_back(S.entity.id);
				}*/
		
		}
		
	}

	for (auto vv : IdleToFight())vReturn.push_back(vv);
	if (Bro->L->TragetSwitcher)for (auto vv : SwitchTargets())vReturn.push_back(vv);

	MISE;
	return vReturn;
}

std::vector<capi::Command> FireBot::sPanicDef()
{
	MISS;
	auto vReturn = std::vector<capi::Command>();
	

	if (iWallReadyTick < lState.current_tick)
	{
		//Find My Wall
		std::vector<capi::Entity> barrier =
			Bro->U->pointsInRadius(entitiesTOentity(0, lState.entities.barrier_sets),
				capi::to2D(eMainOrb.position),
				50);

		if (barrier.size() == 1)
		{
			//Build Wall
			vReturn.push_back(MIS_CommandBarrierBuild(barrier[0].id));
			iSkipTick = lState.current_tick + 10;
			MISEA("First Build Wall");
			iPanicDefCheck++;
			return vReturn;
		}
		else
		{
			iWallReadyTick = 99999; // No Walls around my Main Orb
		}
	}
	
	//Spawn Archers	
	if (iArchers != -1 && lState.players[imyPlayerIDX].power > SMJDeck[iArchers].powerCost)
	{
		vReturn.push_back(MIS_CommandProduceSquad(iArchers, capi::to2D(eMainOrb.position)));
	}

	//Archers on the Wall
	
	std::vector < capi::Entity> myBarrier = entitiesTOentity(myId, lState.entities.barrier_modules);

	if (myBarrier.size() == 0)
	{
		MISEA("WHERE IS MY WALL!!!");
		return vReturn;
	}


	capi::Position GatePos = myBarrier[0].position; //Init with something

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


	bool DoMount;
	for (auto S : Bro->U->SquadsInRadius(myId, lState.entities.squads, capi::to2D(eMainOrb.position), 50))
	{
		//Archer
		if (CARD_ID_to_SMJ_CARD(S.card_id).attackType == 1)
		{
			DoMount = true;
			for (auto I : vIdler)if (I->id == S.entity.id) DoMount = false;
			//Already on wall?
			if(DoMount)
				for (auto A : S.entity.aspects)
					if (A.variant_case == capi::AspectCase::MountBarrier)
						if (A.variant_union.mount_barrier.state.variant_case == capi::MountStateCase::Unmounted)
						{
							capi::Entity A;
							capi::Entity B;
								
							Bro->U->CloseCombi({ S.entity }, myBarrier, A, B);
							vReturn.push_back(MIS_CommandGroupEnterWall({ S.entity.id }, B.id));
							vIdler.push_back(new MIS_Ideler(S.entity.id, lState.current_tick + Bro->L->IdleOffset));
						}
		}
		else if (squadIsIdle(S.entity.id))
			vReturn.push_back(MIS_CommandGroupGoto({ S.entity.id }, capi::to2D(GatePos), capi::WalkMode_Normal));
				
	}
	
	for (auto vv : IdleToFight())vReturn.push_back(vv);
	if (Bro->L->TragetSwitcher)for (auto vv : SwitchTargets())vReturn.push_back(vv);
	
	MISE;
	return vReturn;
}

std::vector<capi::Command> FireBot::sDisablePanicDef()
{
	MISS;
	
	auto vReturn = std::vector<capi::Command>();
	for (auto B : entitiesTOentity(myId, lState.entities.barrier_sets))
		vReturn.push_back(MIS_CommandGroupKillEntity({ B.id }));

	iWallReadyTick = lState.current_tick + 670; //ready in ~65 sec
	iPanicDefCheck = 0;

	MISE;
	return vReturn;
}

std::vector<capi::Command> FireBot::sTierUp()
{
	MISS;

	auto vReturn = std::vector<capi::Command>();

	capi::CreateOrbColor ToBuild;

	capi::Entity A;
	capi::Entity B;
	capi::Entity C;
	float fDistanc = 0;

	MISD("#1");
	//Closest Orb
	fDistanc = Bro->U->CloseCombi(entitiesTOentity(myId, lState.entities.squads, lState.entities.power_slots),
		entitiesTOentity(0, lState.entities.token_slots), A, B);
	MISD("#2");
	//Is OP near that orb?
	if (B.id == 0 && 
		Bro->U->CloseCombi(entitiesTOentity(opId, lState.entities.squads),{ B }, C, B) < Bro->L->SaveRangeWellOrb 
		//If no Tier one - THEN GET ONE!!!
		&& iStageValue != 1)
	{
		MISD("#3");
		//Earyler Check if behind
		if (entitiesTOentity(myId, lState.entities.token_slots).size() < entitiesTOentity(opId, lState.entities.token_slots).size())
			TierCheckTick = lState.current_tick + Bro->L->TierCheckOffset * 0.25;
		else TierCheckTick = lState.current_tick + Bro->L->TierCheckOffset;
	}

	MISD("#3");
	if (A.player_entity_id == myId)
	{
		MISD("#4");
		if (fDistanc > Bro->L->CastRange)vReturn.push_back(MIS_CommandGroupGoto({ A.id }, capi::to2D(B.position), capi::WalkMode_Normal));
		else //if(lState.players[imyPlayerIDX].power >= 153)
		{
			if (lState.players[imyPlayerIDX].orbs.frost > 0) ToBuild = capi::CreateOrbColor::CreateOrbColor_Frost;
			else if (lState.players[imyPlayerIDX].orbs.nature > 0) ToBuild = capi::CreateOrbColor::CreateOrbColor_Nature;
			else if (lState.players[imyPlayerIDX].orbs.shadow > 0) ToBuild = capi::CreateOrbColor::CreateOrbColor_Shadow;
			else ToBuild = capi::CreateOrbColor::CreateOrbColor_Fire;

			MISD("#8");
			vReturn.push_back(capi::Command(MIS_CommandTokenSlotBuild(B.id, ToBuild)));
		}
	}
	MISD("#9");
	for (auto vv : IdleToFight())vReturn.push_back(vv);
	if (Bro->L->TragetSwitcher)for (auto vv : SwitchTargets())vReturn.push_back(vv);

	MISE;
	return vReturn;
}

std::vector<capi::Command> FireBot::sDefaultDef()
{
	MISS;

	auto vReturn = std::vector<capi::Command>();
	capi::Entity A;
	capi::Entity B;

	float fDisntanc;

	

	for (auto V : vBases)
	{
		//ALL Fine at this base
		if (V->PowerLevelSum() >= 0) continue;

		NextCardSpawn = CardPickerFromBT(V->opBattleTable, None);
		if (SMJDeck[NextCardSpawn].powerCost < lState.players[imyPlayerIDX].power)
			vReturn.push_back(
				MIS_CommandProduceSquad(NextCardSpawn, capi::to2D(V->Base.position)));
	}

	//If units fare between 50 and 100 Pull bakc
	// >100 will be send on a atack later
	for (auto eSquat : entitiesTOentity(myId, lState.entities.squads))
	{
		fDisntanc = Bro->U->CloseCombi({ eSquat }, entitiesTOentity(myId, lState.entities.power_slots, lState.entities.token_slots), A, B);
		if (fDisntanc > Bro->L->RetreatRange && fDisntanc < Bro->L->FightRange && squadIsIdle(A.id))
			vReturn.push_back(MIS_CommandGroupGoto({ A.id }, capi::to2D(B.position), capi::WalkMode_Normal));
	}
	
	
	//Ideal Units do stuff
	for (auto vv : IdleToFight())vReturn.push_back(vv);
	if (Bro->L->TragetSwitcher)for (auto vv : SwitchTargets())vReturn.push_back(vv);

				
	MISE;
	return vReturn;
}

bool FireBot::BuildWellOrbCheck()
{
	MISS;

	//Swiming in power
	if (lState.players[imyPlayerIDX].power > 500)
	{
		MISEA("#1 BUILD time");
		return true;
	}

	unsigned int iMyBound = 0;
	unsigned int iOpBound = 0;
	for (auto S : Bro->U->FilterSquad(myId, lState.entities.squads)) iMyBound += CARD_ID_to_SMJ_CARD(S.card_id).powerCost;
	for (auto S : Bro->U->FilterSquad(opId, lState.entities.squads)) iOpBound += CARD_ID_to_SMJ_CARD(S.card_id).powerCost;

	//if OP has wall, units are not as scary
	auto opWall = entitiesTOentity(opId, lState.entities.barrier_modules);
	if (opWall .size() > 0) for(auto S : Bro->U->SquadsInRadius(opId, lState.entities.squads, capi::to2D(opWall[0].position), 150))
		iOpBound -= CARD_ID_to_SMJ_CARD(S.card_id).powerCost * 0.25;

	if ((iMyBound + lState.players[imyPlayerIDX].power + lState.players[imyPlayerIDX].void_power * 0.5) * 0.01 //Fix roundings
		> (iOpBound + lState.players[iopPlayerIDX].power + lState.players[iopPlayerIDX].void_power * 0.5) * 0.05 // 5% more then OP
		)
	{
		MISD(std::to_string((iMyBound + lState.players[imyPlayerIDX].power + lState.players[imyPlayerIDX].void_power * 0.5) * 0.01) + " < " + std::to_string(iOpBound + lState.players[iopPlayerIDX].power + lState.players[iopPlayerIDX].void_power * 0.5));
		//MISEA("Power bad Idea");
		return false;
	}
	MISEA("#2 BUILD time");
	return true;
}

bool FireBot::squadIsIdle(capi::EntityId _ID)
{
	for (auto I : vIdler)if (I->id == _ID) return false;	

	for (auto E : lState.entities.squads)
		if (E.entity.id == _ID)
			if (E.entity.job.variant_case == capi::JobCase::Idle)
				return true;
	return false;
}

std::vector<capi::Command> FireBot::ResetUnits()
{
	auto CommandGroupStopJob = capi::CommandGroupStopJob();
	for (auto E : Bro->U->FilterSquad(myId, lState.entities.squads))
		CommandGroupStopJob.squads.push_back(E.entity.id);
	return { capi::Command(CommandGroupStopJob) };	
}


std::vector<capi::Command> FireBot::SwitchTargets()
{
	auto vReturn = std::vector<capi::Command>();

	bool found;
	bool WallWithSquad;
	capi::Entity A;
	capi::Entity B;
	std::vector<capi::Entity> SquadsInRange;

	//for (auto E : lState.entities.squads)
	for (auto E : Bro->U->FilterSquad(myId, lState.entities.squads))
	{		
		for (auto I : vIdler)if (I->id == E.entity.id)continue;

		capi::EntityId Target = getAttackTargetID(E);
		//When Atacking <> Squats no switch (wells / orbs)
		if (Target == -1)continue;
		

		found = false;
		//First Check for Squads
		for (auto OP : lState.entities.squads)
		{
			
			if (OP.entity.id != Target)continue;			
			found = true;
			//Not perfect Counter
			if (CARD_ID_to_SMJ_CARD(E.card_id).offenseType != CARD_ID_to_SMJ_CARD(OP.card_id).defenseType)		
				for (auto OP2 : Bro->U->SquadsInRadius(opId, lState.entities.squads, capi::to2D(E.entity.position), Bro->L->SwitchTargetRange))
					//Is perfect counter And not on a wall
					if (CARD_ID_to_SMJ_CARD(E.card_id).offenseType == CARD_ID_to_SMJ_CARD(OP2.card_id).defenseType
						&& !onWall(OP2.entity))
					{
						vReturn.push_back(MIS_CommandGroupAttack({ E.entity.id }, OP2.entity.id));
						MISD("SWITCH " + CARD_ID_to_SMJ_CARD(E.card_id).cardName + " to " + CARD_ID_to_SMJ_CARD(OP2.card_id).cardName);
						break;
					}
						
			break;
		}
		//Then check for Walls
		if (found == false && Bro->L->IgnoreEmptyWalls)for (auto OP : lState.entities.barrier_modules)
		{
			if (OP.entity.id != Target)continue;
			found = true;

			WallWithSquad = false;
			SquadsInRange = entitiesTOentity(opId, Bro->U->SquadsInRadius(opId, lState.entities.squads, capi::to2D(E.entity.position), Bro->L->WallSearchRange));
			//Atacks Wall with Squad on Top
			if (SquadsInRange.size() > 0)for (auto S : SquadsInRange)if (Target == WallidOFsquad(S.id))WallWithSquad = true;
			if (WallWithSquad)break;
			
			


			//else find couses unit to atack
			if (SquadsInRange.size() > 0)
			{
				//Cloases Squad around
				Bro->U->CloseCombi({ E.entity }, SquadsInRange, A, B);
				//If on wall then atack wall
				if (onWall(B)) B.id = WallidOFsquad(B.id);				
			}
			else
			{
				//Is the a gap?
				if (! GapInWall(OP.set))break;

				//Search full map for new target
				SquadsInRange = entitiesTOentity(opId, lState.entities.squads, lState.entities.power_slots, lState.entities.token_slots);
				Bro->U->CloseCombi({ E.entity }, SquadsInRange, A, B);
			}

			vReturn.push_back(MIS_CommandGroupAttack({ E.entity.id }, B.id));
			break;
		}

	}

	return vReturn;
}

capi::EntityId FireBot::getAttackTargetID(capi::Squad toCheck)
{
	if (toCheck.entity.job.variant_case != capi::JobCase::AttackSquad)return -1;
	if (toCheck.entity.job.variant_union.attack_squad.target.variant_case != capi::TargetCase::Single)return -1;
	if (toCheck.entity.job.variant_union.attack_squad.target.variant_union.single.single.variant_case != capi::SingleTargetCase::SingleEntity)return -1;

	return toCheck.entity.job.variant_union.attack_squad.target.variant_union.single.single.variant_union.single_entity.id;
}

capi::Entity* FireBot::getAttackTargetEntity(capi::Squad toCheck)
{
	auto CheckID = getAttackTargetID(toCheck);

	if (CheckID == -1) return NULL;

	auto eAllopEnteties = entitiesTOentity(opId, 
		lState.entities.barrier_modules, 
		lState.entities.buildings,
		lState.entities.power_slots,
		lState.entities.token_slots,
		lState.entities.squads);

	for (auto OP : eAllopEnteties)
		if (OP.id == CheckID)return &OP;
	
	return NULL;
}



void FireBot::RemoveFromSaveUnit()
{
	MISS;
	bool bDel;
	std::vector < capi::Entity> MyUnits = entitiesTOentity(myId, lState.entities.squads);
	for (std::vector<capi::EntityId>::iterator it = vSaveUnit.begin(); it != vSaveUnit.end();)
	{
		bDel = true;
		for (auto U : MyUnits)if ((*it) == U.id) bDel = false;

		if (bDel)it = vSaveUnit.erase(it);
		else  ++it;
	}
	MISE;
}

std::vector<capi::Command> FireBot::FixGroupGotoType2()
{
	MISS;
	bool bFixMode;
	
	auto vReturn = std::vector<capi::Command>();
	for (auto S : entitiesTOentity(myId, lState.entities.squads))
	{
		if (S.job.variant_case != capi::JobCase::Goto)continue;
		if (S.job.variant_union.gotoCppField.walk_mode == capi::WalkMode_Force)
		{
			bFixMode = true;
			for (unsigned int i = 0; i < vSaveUnit.size() && bFixMode; i++)
				bFixMode = vSaveUnit[i] != S.id;

			if (bFixMode)
				vReturn.push_back(MIS_CommandGroupGoto({ S.id }, Bro->U->WaypointTo2D(S.job.variant_union.gotoCppField.waypoints[0]), capi::WalkMode_Normal));										
		}
	}
	
	MISE;
	return vReturn;
}


void FireBot::UpdateBases()
{
	MISS;

	bool isNew;
	float HPHelper;

	//Reset Updater
	for (auto B : vBases)
	{		
		B->Update = false;
		B->Alart = false;
	}

	
	for (auto E : entitiesTOentity(myId, lState.entities.power_slots, lState.entities.token_slots))
	{
		UpdateVector:
		isNew = true;
		for (auto B : vBases)
		{
			//If found Update
			if (E.id == B->Base.id)
			{		
				
				B->mySquads = Bro->U->SquadsInRadius(myId, lState.entities.squads, capi::to2D(B->Base.position), Bro->L->RetreatRange);
				B->opSquads = Bro->U->SquadsInRadius(opId, lState.entities.squads, capi::to2D(B->Base.position), Bro->L->DefRange);
				B->myBattleTable = CalcBattleTable(B->mySquads);
				B->opBattleTable = CalcBattleTable(B->opSquads);
				MoreUnitsNeeded(B->myBattleTable, B->opBattleTable, B->PowerLevel);

				HPHelper = GetAspect(E, "Health");
				B->Alart = HPHelper < B->HP;
				B->HP = HPHelper;
				
				B->Update = true;
				isNew = false;
			}
		}

		//If not found -> New so add and Update
		if (isNew)
		{
			vBases.push_back(new BaseDef(E));
			goto UpdateVector;
		}			
	}
	 
	//Remove Old Entries
	for (std::vector<BaseDef *>::iterator it = vBases.begin(); it != vBases.end();)
		if ((*it)->Update == false)it = vBases.erase(it);
		else ++it;

	std::sort(vBases.begin(), vBases.end(), comparePower);

	MISE;
}