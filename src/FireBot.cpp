#define DF_Debug

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

bool comparePower(BaseDef a, BaseDef b)
{
	return a.PowerLevelSum() < b.PowerLevelSum();
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
	iWallReady = -1;
	iMyWells = -1;
	//iMyOrbs = -1;
	iPanicDefCheck = 0;
	TierCheckTick = Bro->L->Tier2Init;
	//bTier2VSWall = true;
	opUsingWalls = false;
	iSkipTick = 0;
	WellCheckTick = 0;

//if mapInfo.map == 1003 -> no Panic Def
	eStage = WaitForOP;
	eStageNext = NoStage;
	iStageValue = 0;
	iStageValueNext = 0;
	bSwitchStrategy = false;

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
		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::DisenchantANature, capi::Upgrade_U3);

		deck.cards[i++] = capi::CardIdWithUpgrade(card_templates::Spitfire, capi::Upgrade_U3);
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
		CoolEruptionTest.s = false;
		Bro->L->UnitEruption = false;
	}
	else CoolEruptionTest.s = true;
	GlobalBattleTable.s = true;
	InstantRepair.s = true;
	Strategy.s = true;	

	MISE;
}

void FireBot::init()
{
	MISS;
	eMainOrb = entitiesTOentity(myId, lState.entities.token_slots)[0];
	MISE;
}

std::vector<capi::Command> FireBot::Tick(const capi::GameState& state)
{		
	lState = state;
	auto v = std::vector<capi::Command>();
	auto vTemp = std::vector<capi::Command>();
	if (state.current_tick == 10)init();

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
			MISERROR("------------------------------");
			MISERROR("# dump     = Save tick        ");
			MISERROR("# dumpx    = Save Commands    ");			
			MISERROR("# bt       = Print BattleTable");
			MISERROR("# st       = echo strategy    ");
			MISERROR("# pl       = echo power level ");
			MISERROR("# i        = Bot Infos        ");
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

		if (Bro->sComand == "pl")
		{
			std::vector<int> plTemp;
			MoreUnitsNeeded(myBT, opBT, plTemp);
			for (auto i : plTemp)MISD(i);
			Bro->sComand = "";
		}

		if (Bro->sComand == "i")
		{
			MISD("current_tick: " + std::to_string(state.current_tick));
			MISD("Power:        " + std::to_string(lState.players[imyPlayerIDX].power));
			MISD("WellCheckTick:" + std::to_string(WellCheckTick));
			MISD("TierReadyTick:" + std::to_string(TierReadyTick));
			MISD("TierCheckTick:" + std::to_string(TierCheckTick));
			
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
	
	if (iWallReady >= 0)iWallReady--;
	if (iPanicDefCheck < 0)iPanicDefCheck++;

		
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
	
	if (Bro->L->BattleTable && GlobalBattleTable.s && state.current_tick % 10 == 1)
	{
		GlobalBattleTable.fb = std::async(&FireBot::CalGlobalBattleTable, this, state);
		GlobalBattleTable.s = false; // dont start again
	}
	
	if (Bro->L->InstantRepair && InstantRepair.s && state.current_tick % 20 == 1)
	{
		InstantRepair.fc = std::async(&FireBot::InstantRepairFunction, this, state);
		InstantRepair.s = false; // dont start again
	}

	////////////////Strategy ////////////////////
	
	if (Strategy.s && state.current_tick % 10 == 1)
	{
		Strategy.fb = std::async(&FireBot::CalcStrategy, this, state);
		Strategy.s = false; // dont start again
	}
	
	if (iSkipTick < state.current_tick) switch (eStage)
	{
		///case WaitForOP:
			//for (auto vv : sWaitForOP())v.push_back(vv);
			//break;
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
		case TierUp:
			for (auto vv : sTierUp())v.push_back(vv);
			break;		
		case DefaultDef:
		case WaitTier:
			for (auto vv : sDefaultDef())v.push_back(vv);
			break;
	}
	
	if (Strategy.s == false)
		if (Strategy.fb.wait_for(0ms) == std::future_status::ready)
		{
			//if (Strategy.fb.get())SwitchStrategy();
			if (bSwitchStrategy)
			{
				SwitchStrategy();
				if(Bro->L->ResetUnitsOnStratSwitch)for (auto vv : ResetUnits())v.push_back(vv);
			}
			Strategy.s = true;
		}
		

	/////////////Cleanups////////////////
	if (Bro->L->AvoidArea)if (vAvoid.size() > 0)RemoveFromMIS_AvoidArea(state.current_tick);
	if (state.current_tick % 100 == 1)RemoveFromSaveUnit();
	if (state.current_tick % 10 == 1)for (auto vv : FixGroupGotoType2())v.push_back(vv); 
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


void FireBot::SwitchStrategy()
{
	MISS;
	//MISERROR("Switch: " + std::to_string(eStage) + " -> " + std::to_string(eStageNext));
	//MISERROR("Switch: " + SwitchStagesText(eStage) + " -> " + SwitchStagesText(eStageNext));
	//MISERROR("Switch: " + std::to_string(iStageValue) + " -> " + std::to_string(iStageValueNext));
	MISERROR((std::to_string(lState.current_tick) + " Switch: " + SwitchStagesText(eStage) + " (" + std::to_string(iStageValue) + ") -> " + SwitchStagesText(eStageNext) + " ("  + std::to_string(iStageValueNext) + ")").c_str());
	eStage = eStageNext;
	iStageValue = iStageValueNext;
	bSwitchStrategy = false;
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

	if (
		OrbOnebOK() && // Is Main Orb is sill alive?
		mapinfo.map == 1003 && // only for event
		iPanicDefCheck >= 0 &&
		eStage != PanicDef)
		if (Bro->U->pointsInRadius(entitiesTOentity(opId, StrategyState.entities.squads),
			capi::to2D(eMainOrb.position),
			100).size() >= 3
			&& Bro->U->pointsInRadius(entitiesTOentity(myId, StrategyState.entities.squads),
				capi::to2D(eMainOrb.position),
				100).size() <= 1
			//Orb Still alive?
			&& Bro->U->pointsInRadius(entitiesTOentity(myId, StrategyState.entities.token_slots),
				capi::to2D(eMainOrb.position),
				10).size() > 0
			)
		{
			SetNextStrategy(PanicDef, 0);
			MISEA("PRIO Return 1");
			return true;
		}

	//OP builds wall
	if (opUsingWalls == false &&
		entitiesTOentity(opId, StrategyState.entities.barrier_modules).size() > 1 &&
		entitiesTOentity(myId, StrategyState.entities.token_slots).size() == 1 &&
		eStage == Fight)
	{
		SetNextStrategy(TierUp, 99); // Def for 125 energy
		TierCheckTick = 0;
		Bro->L->TierCheckOffset = Bro->L->TierCheckOffset / 3; // CHeck for Tier two more often
		opUsingWalls = true;
		MISEA("PRIO Return 2");
		return true;
	}
	/*
	if (entitiesTOentity(opId, StrategyState.entities.barrier_modules).size() > 1 && 
		entitiesTOentity(myId, StrategyState.entities.token_slots).size() == 1 && 
		eStage == Fight && 
		eStage != TierUp &&
		bTier2VSWall)
	{			
		SetNextStrategy(TierUp, 99); // Def for 125 energy
		TierCheckTick = 0;
		Bro->L->TierCheckOffset = Bro->L->TierCheckOffset / 2; // CHeck for Tier two more often
		bTier2VSWall = false;
		MISEA("PRIO Return 2");
		return true;
	}*/

	// Lost my Tier 1!!!
	if (entitiesTOentity(myId, StrategyState.entities.token_slots).size() < 1 &&
		eStage != TierUp)
	{
		SetNextStrategy(TierUp, 1);
		MISEA("PRIO Return 3");
		return true;
	}

	if (eStage != BuildWell && iMyWells != -1)iMyWells = -1;
	//if (eStage != TierUp    && iMyOrbs  != -1)iMyOrbs  = -1;
	

	///////////////////////////////////////////////////////
	/// Default Stage based on currend Stage 1 to <ANY> ///
	///////////////////////////////////////////////////////
	switch (eStage)
	{
	case WaitForOP:
		if (StrategyState.current_tick >= 100)SetNextStrategy(GetUnit, Bro->L->WaitSpawnTime);
		if (StrategyState.entities.squads.size() > 0)SetNextStrategy(GetUnit, Bro->L->WaitSpawnTime);
		break;
	
	case SavePower:
	case DefaultDef:	
		if ((int)StrategyState.players[imyPlayerIDX].power > iStageValue)SetNextStrategy(Fight, 2);
		break;
		
	case PanicDef:
		if (Bro->U->pointsInRadius(entitiesTOentity(opId, StrategyState.entities.squads),
			capi::to2D(eMainOrb.position),
			100).size() == 0)SetNextStrategy(DisablePanicDef, 0);
		if (iPanicDefCheck >= 3)
		{
			iPanicDefCheck = -600;
			SetNextStrategy(DefaultDef, 52);
		}
			break;
	case GetUnit: 
		if (entitiesTOentity(myId, StrategyState.entities.squads).size() > 0)SetNextStrategy(Fight, 1);
		break;
		
	case DisablePanicDef:
		if(entitiesTOentity(myId, StrategyState.entities.barrier_sets).size() == 0)SetNextStrategy(Fight, 3);
		break;	
	case TierUp:
		if (entitiesTOentity(myId, StrategyState.entities.squads).size() == 0)SetNextStrategy(GetUnit, Bro->L->WaitSpawnTime);
		//if (iMyOrbs != -1 && iMyOrbs != entitiesTOentity(myId, StrategyState.entities.token_slots).size())SetNextStrategy(WaitTier, iMyOrbs);
		for (auto O : entitiesTOentity(myId, StrategyState.entities.token_slots))if (O.job.variant_case == capi::JobCase::Construct)
		{
			TierReadyTick = StrategyState.current_tick + Bro->L->OrbBuildWait;
			SetNextStrategy(WaitTier, entitiesTOentity(myId, StrategyState.entities.token_slots).size());
		}


		MISD("TierReadyTick=" + std::to_string(TierReadyTick));
		MISD("TierCheckTick=" + std::to_string(TierCheckTick));
		MISD("current_tick =" + std::to_string(StrategyState.current_tick));
		//if(entitiesTOentity(myId, StrategyState.entities.token_slots).size() == 2)SetNextStrategy(WaitTier2, OrbBuildWait);			
		if (TierReadyTick > StrategyState.current_tick)SetNextStrategy(WaitTier, iStageValue);
		if (TierCheckTick > StrategyState.current_tick)SetNextStrategy(Fight, 6);
		if(BuildWellOrbCheck()== false && iStageValue != 99 && iStageValue != 1)SetNextStrategy(Fight, 4);
		break;
	case WaitTier:
		if (TierReadyTick < StrategyState.current_tick)SetNextStrategy(Fight, 5);
		//if (entitiesTOentity(myId, StrategyState.entities.token_slots).size() >= iStageValue || iStageValue == 99)SetNextStrategy(Fight, 6);
		break;

	case BuildWell:
		if (entitiesTOentity(myId, StrategyState.entities.squads).size() == 0)SetNextStrategy(GetUnit, Bro->L->WaitSpawnTime);
		if (WellCheckTick > StrategyState.current_tick)SetNextStrategy(Fight, 7);
		if (iMyWells != -1 && iMyWells != entitiesTOentity(myId, StrategyState.entities.power_slots).size())SetNextStrategy(DefaultDef, 53);
		//if (WellCheckTick > StrategyState.current_tick)SetNextStrategy(Fight, 8);
		if(BuildWellOrbCheck() == false)SetNextStrategy(DefaultDef, 54);
		break;
	case SpamBotX: 
	case Fight: 
		if (entitiesTOentity(myId, StrategyState.entities.squads).size() == 0)SetNextStrategy(GetUnit, Bro->L->WaitSpawnTime);
		for (auto B : entitiesTOentity(myId, StrategyState.entities.power_slots, StrategyState.entities.token_slots))
			if(Bro->U->SquadsInRadius(opId, StrategyState.entities.squads, capi::to2D(B.position), Bro->L->SwitchToDefRange).size() > 0)
				SetNextStrategy(DefaultDef, 75);

		if (BuildWellOrbCheck() )			
		{
			if (entitiesTOentity(myId, StrategyState.entities.token_slots).size() < 2)
			{
				if (TierCheckTick < StrategyState.current_tick)SetNextStrategy(TierUp, 2);
				else TierCheckTick = StrategyState.current_tick + Bro->L->TierCheckOffset;
			}
			//Tier 3
			else if(Bro->L->Tier3Init < StrategyState.current_tick)SetNextStrategy(TierUp, 3);
		}

		if (BuildWellOrbCheck() && 
				// Max 4 Wells
			(  entitiesTOentity(myId, StrategyState.entities.power_slots).size() < 4 
				//Stay + 1
			|| entitiesTOentity(myId, StrategyState.entities.power_slots).size() <= entitiesTOentity(opId, StrategyState.entities.power_slots).size())
			&& WellCheckTick < StrategyState.current_tick)SetNextStrategy(BuildWell, 2);
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
				if (std::accumulate(PowerLevel.begin(), PowerLevel.end(), 0) < Bro->L->GiveUpFight)SetNextStrategy(DefaultDef, 152);
			}

		break;
	}
	
	MISE;
	return false;
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

	//Is OP near that WELL?
	if (Bro->U->CloseCombi(entitiesTOentity(opId, lState.entities.squads), { B }, C, B) < Bro->L->SaveRangeWellOrb)
	{
		WellCheckTick = lState.current_tick + Bro->L->WellCheckOffset;
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

std::vector<capi::Command> FireBot::sSpamBotX()
{
	MISS;

	auto vReturn = std::vector<capi::Command>();

	capi::Entity A;
	capi::Entity B;	
	int iCard = CardPickerFromBT(opBT, None);

	float fDistanc = Bro->U->CloseCombi(entitiesTOentity(myId, lState.entities.squads, lState.entities.buildings, lState.entities.power_slots, lState.entities.token_slots),
		entitiesTOentity(opId, lState.entities.power_slots, lState.entities.token_slots), A, B);

	if (SMJDeck[iCard].powerCost > lState.players[imyPlayerIDX].power)
	{
		//Cant Spawn insice the base (collition)
		if (fDistanc > Bro->L->CastRange * 2) fDistanc = Bro->L->CastRange;
		else fDistanc = Bro->L->CastRange * -1;

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

		if (S.entity.job.variant_case == capi::JobCase::AttackSquad || fDistanc < Bro->L->FightRange) // Unit in Combet pr near
		{
			/*
			//Check if the target is near, else set to Idel for a tick to pick new target
			auto eTarget = getAttackTargetEntity(S);
			if (eTarget != NULL)
				if (Bro->U->distance(capi::to2D(S.entity.position), capi::to2D(eTarget->position)) > fDistanc * 1.25)
					vReturn.push_back(MIS_CommandGroupStopJob({ S.entity.id }));*/
						
			//Fall Back with Archers
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
				//MISD("DANCE");
				//MISD(NextCardSpawn);
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
					fDistanc = Bro->U->CloseCombi(entitiesTOentity(myId, Bro->U->SquadsInRadius(myId, lState.entities.squads, capi::to2D(S.entity.position), 75)),
						entitiesTOentity(opId, Bro->U->SquadsInRadius(opId, lState.entities.squads, capi::to2D(S.entity.position), Bro->L->FightRange)), A, B);
					if (fDistanc > Bro->L->CastRange)fDistanc = Bro->L->CastRange;
					SpawnPos = Bro->U->Offseter(capi::to2D(A.position), capi::to2D(B.position), fDistanc - 1, -1); //a bit offset
				}
				vReturn.push_back(MIS_CommandProduceSquad(NextCardSpawn, SpawnPos));
#ifdef MIS_DEBUG
				if (Bro->L->DrawSpawn)vReturn.push_back(MIS_Ping_Attention(SpawnPos));
#endif // MIS_DEBUG	
			}
		}

		for (auto vv : IdleToFight())vReturn.push_back(vv);
		if(Bro->L->TragetSwitcher)for (auto vv : SwitchTargets())vReturn.push_back(vv);
	}
	MISE;
	return vReturn;
}

std::vector<capi::Command> FireBot::sPanicDef()
{
	MISS;
	auto vReturn = std::vector<capi::Command>();

	if (iWallReady == -1)
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
			iWallReady = -2;
		}
	}
	
	//Spawn Archers	
	if (lState.players[imyPlayerIDX].power > SMJDeck[iArchers].powerCost)
	{
		vReturn.push_back(MIS_CommandProduceSquad(iArchers, capi::to2D(eMainOrb.position)));
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


	
	for (auto S : Bro->U->pointsInRadius(entitiesTOentity(myId, lState.entities.squads), capi::to2D(eMainOrb.position), 50))
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

	iWallReady = 70 * 10; //ready in ~65 sec
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

	//Closest Orb
	fDistanc = Bro->U->CloseCombi(entitiesTOentity(myId, lState.entities.squads, lState.entities.power_slots),
		entitiesTOentity(0, lState.entities.token_slots), A, B);

	//Is OP near that orb?
	if (Bro->U->CloseCombi(entitiesTOentity(opId, lState.entities.squads),{ B }, C, B) < Bro->L->SaveRangeWellOrb 
		//If no Tier one - THEN GET ONE!!!
		&& iStageValue != 1)
	{
		//Earyler Check if behind
		if (entitiesTOentity(myId, lState.entities.token_slots).size() < entitiesTOentity(opId, lState.entities.token_slots).size())
			TierCheckTick = lState.current_tick + Bro->L->TierCheckOffset * 0.25;
		else TierCheckTick = lState.current_tick + Bro->L->TierCheckOffset;
	}

	//if (A.player_entity_id == myId)
	//{
		if (fDistanc > Bro->L->CastRange)vReturn.push_back(MIS_CommandGroupGoto({ A.id }, capi::to2D(B.position), capi::WalkMode_Normal));
		else //if(lState.players[imyPlayerIDX].power >= 153)
		{
			if (lState.players[imyPlayerIDX].orbs.frost > 0) ToBuild = capi::CreateOrbColor::CreateOrbColor_Frost;
			else if (lState.players[imyPlayerIDX].orbs.nature > 0) ToBuild = capi::CreateOrbColor::CreateOrbColor_Nature;
			else if (lState.players[imyPlayerIDX].orbs.shadow > 0) ToBuild = capi::CreateOrbColor::CreateOrbColor_Shadow;
			else ToBuild = capi::CreateOrbColor::CreateOrbColor_Fire;

			vReturn.push_back(capi::Command(MIS_CommandTokenSlotBuild(B.id, ToBuild)));
		}
	//}

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
	
	std::vector<BaseDef> vBase;

	for (auto E : entitiesTOentity(myId, lState.entities.power_slots, lState.entities.token_slots))
	{
		BaseDef TempBase;
		TempBase.Base = E;
		TempBase.mySquads = Bro->U->SquadsInRadius(myId, lState.entities.squads, capi::to2D(E.position), Bro->L->RetreatRange);
		TempBase.opSquads = Bro->U->SquadsInRadius(opId, lState.entities.squads, capi::to2D(E.position), Bro->L->FightRange);
		TempBase.myBattleTable = CalcBattleTable(TempBase.mySquads);
		TempBase.opBattleTable = CalcBattleTable(TempBase.opSquads);
		MoreUnitsNeeded(TempBase.myBattleTable, TempBase.opBattleTable, TempBase.PowerLevel);

		vBase.push_back(TempBase);
	}
	
	std::sort(vBase.begin(), vBase.end(), comparePower);

	for (auto V : vBase)
	{
		//ALL Fine at this base
		if (V.PowerLevelSum() >= 0) continue;

		NextCardSpawn = CardPickerFromBT(V.opBattleTable, None);
		if (SMJDeck[NextCardSpawn].powerCost < lState.players[imyPlayerIDX].power)
			vReturn.push_back(
				MIS_CommandProduceSquad(NextCardSpawn, capi::to2D(V.Base.position)));
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
	for (auto S : Bro->U->FilterSquad(myId, lState.entities.squads)) iMyBound += Bro->J->CardFromJson(S.card_id % 1000000).powerCost;
	for (auto S : Bro->U->FilterSquad(opId, lState.entities.squads)) iOpBound += Bro->J->CardFromJson(S.card_id % 1000000).powerCost;

	//if OP has wall, units are not as scary
	auto opWall = entitiesTOentity(opId, lState.entities.barrier_modules);
	if (opWall .size() > 0) for(auto S : Bro->U->SquadsInRadius(opId, lState.entities.squads, capi::to2D(opWall[0].position), 150))
		iOpBound -= Bro->J->CardFromJson(S.card_id % 1000000).powerCost * 0.25;

	if ((iMyBound + lState.players[imyPlayerIDX].power + lState.players[imyPlayerIDX].void_power * 0.5) * 0.01 //Fix roundings
		< iOpBound + lState.players[iopPlayerIDX].power + lState.players[iopPlayerIDX].void_power * 0.5
		)
	{
		//MISD(std::to_string(iMyBound + lState.players[imyPlayerIDX].power + lState.players[imyPlayerIDX].void_power * 0.5) + " < " + std::to_string(iOpBound + lState.players[iopPlayerIDX].power + lState.players[iopPlayerIDX].void_power * 0.5));
		MISEA("Power bad Idea");
		return false;
	}
	MISEA("#2 BUILD time");
	return true;
}

bool FireBot::squadIsIdle(capi::EntityId _ID)
{
	for (auto E : lState.entities.squads)
		if (E.entity.id == _ID)
			if (E.entity.job.variant_case == capi::JobCase::Idle)
				return true;
	return false;
}

std::vector<capi::Command> FireBot::ResetUnits()
{
	//MISD("RESET")
	auto vReturn = std::vector<capi::Command>();
	for (auto E : Bro->U->FilterSquad(myId, lState.entities.squads))
		vReturn.push_back(MIS_CommandGroupStopJob({ E.entity.id }));	
	return vReturn;
}


std::vector<capi::Command> FireBot::SwitchTargets()
{
	auto vReturn = std::vector<capi::Command>();

	//for (auto E : lState.entities.squads)
	for (auto E : Bro->U->FilterSquad(myId, lState.entities.squads))
	{		
		capi::EntityId Target = getAttackTargetID(E);
		//When Atacking <> Squats no switch (wells / orbs)
		for (auto OP : lState.entities.squads)
		{
			if (OP.entity.id != Target)continue;

			//Not perfect Counter
			if (CARD_ID_to_SMJ_CARD(E.card_id).offenseType != CARD_ID_to_SMJ_CARD(OP.card_id).defenseType)		
				for (auto OP2 : Bro->U->SquadsInRadius(opId, lState.entities.squads, capi::to2D(E.entity.position), Bro->L->SwitchTargetRange))
					//Is perfect counter
					if (CARD_ID_to_SMJ_CARD(E.card_id).offenseType == CARD_ID_to_SMJ_CARD(OP2.card_id).defenseType)
					{
						vReturn.push_back(MIS_CommandGroupAttack({ E.entity.id }, OP2.entity.id));
						MISD("SWITCH " + CARD_ID_to_SMJ_CARD(E.card_id).cardName + " to " + CARD_ID_to_SMJ_CARD(OP2.card_id).cardName);
						break;
					}
						
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