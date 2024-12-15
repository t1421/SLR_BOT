#pragma once

#include "../incl/Broker.h"
#include "../incl/DEBUG.h"
#include "../incl/DataTypes.h"

#include "../API/API/json.hpp"
#include "../API/API/TypesCLike.hpp"
#include "../API/API/HelpersCLike.hpp"

#include "../API/API/boost_wrapper.hpp"

#include <future>
#include <chrono>
#include <string>
using namespace std::chrono_literals;

enum Stages
{	
	BuildWell = 10,
	Attack = 13,
	Fight = 14,
	SavePower = 15,

	WaitForOP = 20,
	GetUnit = 21,

	PanicDef = 30,
	DisablePanicDef = 31,
	DefaultDef = 32,

	TierUp = 40,
	WaitTier = 43,

	NoStage = 99
};

std::string SwitchStagesText(Stages S)
{
	switch (S) {
	case 10: return "BuildWell";
	case 13: return "Attack   ";
	case 14: return "Fight    ";
	case 15: return "SavePower";

	case 20: return "WaitForOP";
	case 21: return "GetUnit  ";

	case 30: return "PanicDef ";
	case 31: return "DisPanDef";
	case 32: return "DefaDef  ";

	case 40: return "TierUp   ";
	case 43: return "WaitTier ";

	case 99: return "NoStage  ";
	default: return "Undefined";
	}
}

enum CardPickCrit
{
	None = 0,
	Swift = 1,
	NotS = 2,
	NotM = 3,
	NotL = 4,
	Archer = 5,
	IgnorTier = 6,
	Siege = 7
};


struct MIS_AvoidArea
{
	MIS_AvoidArea(capi::Tick _endTick, capi::Position2D _pos, float _radius) : endTick(_endTick), pos(_pos), radius(_radius) {};
	MIS_AvoidArea(capi::Tick _endTick, capi::Position2D _pos, float _radius, std::string _note) : endTick(_endTick), pos(_pos), radius(_radius), note(_note) { };
	capi::Tick endTick;
	capi::Position2D pos;
	float radius;
	std::string note;
};

struct MIS_Ideler
{
	MIS_Ideler(capi::EntityId _id, capi::Tick _endTick) : id(_id),endTick(_endTick){};
	capi::EntityId id;
	capi::Tick endTick;	
};


struct MIS_RejectCheck
{
	capi::Command orgCommand;
	capi::Command lastCommand;
	unsigned int retry;
	bool rejected;

	MIS_RejectCheck(capi::Command _C) :orgCommand(_C), lastCommand(_C), retry(0), rejected(false) {};
};

struct BaseDef
{
	capi::Entity Base;
	std::vector<capi::Squad> mySquads;
	std::vector<capi::Squad> opSquads;
	BattleTable myBattleTable;
	BattleTable opBattleTable;
	std::vector<int> PowerLevel;
	int PowerLevelSum() { return std::accumulate(PowerLevel.begin(), PowerLevel.end(), 0); };
};

/////////////////
capi::Command MIS_CommandGroupAttack(std::vector<capi::EntityId> _Units, capi::EntityId _OP)
{
	auto GroupAttack = capi::CommandGroupAttack();
	GroupAttack.squads = _Units;
	GroupAttack.target_entity_id = _OP;
	return capi::Command(GroupAttack);
}
capi::Command MIS_CommandProduceSquad(uint8_t _Card, capi::Position2D _Pos)
{
	auto ProduceSquad = capi::CommandProduceSquad();
	ProduceSquad.card_position = _Card;
	ProduceSquad.xy = _Pos;
	return capi::Command(ProduceSquad);
}
capi::Command MIS_CommandGroupGoto(std::vector<capi::EntityId> _Units, capi::Position2D _Pos, capi::WalkMode _Type)
{
	auto GroupGoto = capi::CommandGroupGoto();
	GroupGoto.squads = _Units;
	GroupGoto.positions = { _Pos };
	GroupGoto.walk_mode = _Type;
	return capi::Command(GroupGoto);
}
capi::Command MIS_CommandPowerSlotBuild(capi::EntityId _ID)
{
	auto PowerSlotBuild = capi::CommandPowerSlotBuild();
	PowerSlotBuild.slot_id = _ID;
	return capi::Command(PowerSlotBuild);
}
capi::Command MIS_CommandBarrierBuild(capi::EntityId _ID)
{
	auto BarrierBuild = capi::CommandBarrierBuild();
	BarrierBuild.barrier_id = _ID;
	return capi::Command(BarrierBuild);
}
capi::Command MIS_CommandGroupEnterWall(std::vector<capi::EntityId> _Units, capi::EntityId _ID)
{
	auto GroupEnterWall = capi::CommandGroupEnterWall();
	GroupEnterWall.squads = _Units;
	GroupEnterWall.barrier_id = _ID;
	return capi::Command(GroupEnterWall);
}
capi::Command MIS_CommandBarrierGateToggle(capi::EntityId _ID)
{
	auto BarrierGateToggle = capi::CommandBarrierGateToggle();
	BarrierGateToggle.barrier_id = _ID;
	return capi::Command(BarrierGateToggle);
}
capi::Command MIS_CommandGroupKillEntity(std::vector<capi::EntityId> _Units)
{
	auto GroupKillEntity = capi::CommandGroupKillEntity();
	GroupKillEntity.entities = _Units;
	return capi::Command(GroupKillEntity);
}
capi::Command MIS_CommandRepairBuilding(capi::EntityId _building_id)
{
	auto RepairBuilding = capi::CommandRepairBuilding();
	RepairBuilding.building_id = _building_id;
	return capi::Command(RepairBuilding);
}
capi::Command MIS_CommandTokenSlotBuild(capi::EntityId _ID, capi::CreateOrbColor _Color)
{
	auto TokenSlotBuild = capi::CommandTokenSlotBuild();
	TokenSlotBuild.slot_id = _ID;
	TokenSlotBuild.color = _Color;
	return capi::Command(TokenSlotBuild);
}
capi::Command MIS_Ping_Attention(capi::Position2D _Pos)
{
	auto Ping = capi::CommandPing();
	Ping.ping = capi::Ping::Ping_Attention;
	Ping.xy = _Pos;	
	return capi::Command(Ping);
}
capi::Command MIS_CommandGroupStopJob(std::vector<capi::EntityId> _Units)
{
	auto CommandGroupStopJob = capi::CommandGroupStopJob();
	CommandGroupStopJob.squads = _Units;	
	return capi::Command(CommandGroupStopJob);
}

capi::Command MIS_CommandBarrierRepair(capi::EntityId _barrier_id)
{
	auto BarrierRepair = capi::CommandBarrierRepair();
	BarrierRepair.barrier_id = _barrier_id;
	return capi::Command(BarrierRepair);
}
/////////////////


// /AI: add FireBot FireDeck 4
class FireBot : public capi::IBotImp
{
public:
	FireBot(std::string sBotNme) : capi::IBotImp(sBotNme), myId{}, opId{}, eStage(BuildWell), iSkipTick(0), imyPlayerIDX(0), iopPlayerIDX(0), MaxAvoidID(0){ };
	~FireBot() override = default;
	std::vector<capi::Deck> DecksForMap(const capi::MapInfo& mapInfo) override;
	void PrepareForBattle(const capi::MapInfo& mapInfo, const capi::Deck& deck) override;
	void MatchStart(const capi::GameStartState& state) override;
	std::vector<capi::Command> Tick(const capi::GameState& state) override;

	//const std::string Name;

	static broker* Bro;
	static void learnBro(broker* _Bro) { Bro = _Bro; }

private:
	capi::GameState lState;
	capi::EntityId myId;
	capi::EntityId opId;
	capi::EntityId MaxAvoidID;
	capi::Entity eMainOrb;
	capi::MapInfo mapinfo;

	std::vector<capi::EntityId> vSaveUnit;
	std::vector<MIS_Ideler *> vIdler;
	std::vector<MIS_RejectCheck> RejectedComamandChecklist;
	std::vector<Card> SMJDeck;
	std::vector<Card> SMJDeckOP;

	BattleTable myBT;
	BattleTable opBT;

	bool opUsingWalls;
	unsigned int iSkipTick;
	unsigned int iArchers;
	unsigned int imyPlayerIDX;
	unsigned int iopPlayerIDX;
	unsigned long int WellCheckTick;
	unsigned long int TierReadyTick;
	unsigned long int TierCheckTick;
	unsigned long int iWallReadyTick;
	int iPanicDefCheck;
	int EruptionPos;	
	
	int iMyWells;
	int NextCardSpawn;
	

	//Avoid Area
	void FindAvoidArea();
	void RemoveFromMIS_AvoidArea(capi::Tick curTick);
	std::vector<capi::Command> MoveUnitsAway();
	std::vector<MIS_AvoidArea *> vAvoid;
	
	// BattleTable
	BattleTable CalcBattleTable(std::vector<capi::Squad> squads);
	void EchoBattleTable(BattleTable BT);
	void CalGlobalBattleTable();
	bool MoreUnitsNeeded(BattleTable myBT, BattleTable opBT);
	bool MoreUnitsNeeded(BattleTable myBT, BattleTable opBT, std::vector<int> &PowerLevel);
	int CardPickerFromBT(BattleTable BT, CardPickCrit Crit);
	int CardPicker(unsigned int opSize, unsigned int opCounter, CardPickCrit Crit);

	//Move Units away
	void RemoveFromSaveUnit();

	//Reject Commands
	void CleanUpRejectedComamandChecklist();
	std::vector<capi::Command> Handel_CardRejected_ProduceSquad(capi::Command cIN);
	
	//Util FUnctions Fucntions
	capi::EntityId WallidOFsquad(capi::EntityId ID);
	capi::EntityId getAttackTargetID(capi::Squad toCheck);
	capi::Entity* getAttackTargetEntity(capi::Squad toCheck);
	capi::EntityId behindWall(capi::Entity myUnit, capi::Entity opUnit);
	capi::EntityId SetOfWallmodul(capi::EntityId E);
	Card CARD_ID_to_SMJ_CARD(capi::CardId card_id);
	bool BuildWellOrbCheck();		
	bool OrbOneOK();
	bool squadIsIdle(capi::EntityId _ID);
	bool onWall(capi::Entity E);
	
	bool GapInWall(capi::EntityId E);
	float GetSquadHP(capi::EntityId SquadID);
	float GetSquadMAXHP(capi::EntityId SquadID);
	float GetAspect(capi::Entity E, std::string Asp);
	

	//Extra Fuctions
	std::vector<capi::Command> InstantRepairFunction();
	std::vector<capi::Command> WellKiller();
	std::vector<capi::Command> CoolEruption();
	

	//Support Fctions
	std::vector<capi::Command> SwitchTargets();
	std::vector<capi::Command> IdleToFight();
	std::vector<capi::Command> FixGroupGotoType2();
	std::vector<capi::Command> ResetUnits();

	//STAGE
	Stages eStage;
	Stages eStageNext;
	int iStageValue;
	int iStageValueNext;
	void SwitchStrategy();
	bool SetNextStrategy(Stages _Stage, int _Value) { eStageNext = _Stage; iStageValueNext = _Value; return true; };
	bool CalcStrategy(const capi::GameState& StrategyState);
	
	std::vector<capi::Command> sBuildWell();
	std::vector<capi::Command> sGetUnit();
	std::vector<capi::Command> sFight();
	std::vector<capi::Command> sPanicDef();
	std::vector<capi::Command> sDisablePanicDef();
	std::vector<capi::Command> sTierUp();
	std::vector<capi::Command> sDefaultDef();

	
};
