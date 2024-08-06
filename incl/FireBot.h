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
	BuildWell = 1,
	Attack = 3,
	Fight = 4,
	SavePower = 5,

	WaitForOP = 10,
	GetUnit = 11,
	SpamBotX = 12,

	PanicDef = 20,
	DisablePanicDef = 21,
	DefaultDef = 22,

	Tier2 = 30,
	Tier1 = 31,

	NoStage = 99
};

std::string SwitchStagesText(Stages S)
{
	switch (S) {
	case 1: return "BuildWell";
	case 3: return "Attack";
	case 4: return "Fight";
	case 5: return "SavePower";

	case 10: return "WaitForOP";
	case 11: return "GetUnit";
	case 12: return "SpamBotX";

	case 20: return "PanicDef";
	case 21: return "DisablePanicDef";
	case 22: return "DefaultDef";

	case 30: return "Tier2";
	case 31: return "Tier1";

	case 99: return "NoStage";
	default: return "Undefined in SwitchStagesText";
	}
}

enum CardPickCrit
{
	None = 0,
	Swift = 1,
	NotS = 2,
	NotM = 3,
	NotL = 4,
	Archer = 5
};

struct MIS_thread
{
	std::future<std::vector<capi::Command>> fc;
	std::future<bool> fb;
	//std::thread t;
	bool s;

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


struct MIS_RejectCheck
{
	capi::Command orgCommand;
	capi::Command lastCommand;
	unsigned int retry;
	bool rejected;

	MIS_RejectCheck(capi::Command _C) :orgCommand(_C), lastCommand(_C), retry(0), rejected(false) {};
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


/////////////////


// /AI: add FireBot FireDeck 4
class FireBot : public capi::IBotImp
{
public:
	FireBot(std::string sBotNme) : capi::IBotImp(sBotNme), myId{}, opId{}, myStart{}, eStage(BuildWell), iSkipTick(0), imyPlayerIDX(0), iopPlayerIDX(0), MaxAvoidID(0){ };
	~FireBot() override = default;
	std::vector<capi::Deck> DecksForMap(const capi::MapInfo& mapInfo) override;
	void PrepareForBattle(const capi::MapInfo& mapInfo, const capi::Deck& deck) override;
	void MatchStart(const capi::GameStartState& state) override;
	std::vector<capi::Command> Tick(const capi::GameState& state) override;

	const std::string Name;

	static broker* Bro;
	static void learnBro(broker* _Bro) { Bro = _Bro; }

private:
	capi::GameState lState;

	std::vector<Card> SMJDeck;
	std::vector<Card> SMJDeckOP;
	capi::EntityId myId;
	capi::EntityId opId;
	unsigned int imyPlayerIDX;
	unsigned int iopPlayerIDX;
	BattleTable myBT;
	BattleTable opBT;
	capi::Position2D myStart;

	capi::MapInfo mapinfo;

	unsigned int iSkipTick;

	//Instand Functions
	bool WellKiller(std::vector<capi::Command> &vMain, std::vector<capi::Entity> Wells);
	void FindAvoidArea();
	void RemoveFromMIS_AvoidArea(capi::Tick curTick);
	std::vector<capi::Command> MoveUnitsAway();
	std::vector<MIS_AvoidArea *> vAvoid;
	capi::EntityId MaxAvoidID;
	

	//
	int CardPickerFromBT(BattleTable BT, CardPickCrit Crit, unsigned int Tier);
	int CardPicker(unsigned int opSize, unsigned int opCounter, CardPickCrit Crit, unsigned int Tier);

	std::vector<capi::Command> CoolEruption(const capi::GameState& state);
	MIS_thread CoolEruptionTest;
	int EruptionPos;

	BattleTable CalcBattleTable(std::vector<capi::Squad> squads);
	bool CalGlobalBattleTable(const capi::GameState& state);
	MIS_thread GlobalBattleTable;
	void EchoBattleTable(BattleTable BT);
	int NextCardSpawn;
	bool MoreUnitsNeeded(BattleTable myBT, BattleTable opBT);
	bool MoreUnitsNeeded(BattleTable myBT, BattleTable opBT, std::vector<int> &PowerLevel);

	std::vector<capi::Command> InstantRepairFunction(const capi::GameState& state);
	MIS_thread InstantRepair;

	void RemoveFromSaveUnit();
	std::vector< capi::EntityId> vSaveUnit;
	Card CARD_ID_to_SMJ_CARD(capi::CardId card_id);

	int iWallReady;
	int iMyWells;
	int iPanicDefCheck;
	int iTierReady;

	std::vector<MIS_RejectCheck> RejectedComamandChecklist;
	void CleanUpRejectedComamandChecklist();
	std::vector<capi::Command> Handel_CardRejected_ProduceSquad(capi::Command cIN); // , std::vector<capi::Command>& addHere);
	
	std::vector<capi::Command>  IdleToFight();
	float GetAspect(capi::Entity E, capi::AspectCase A);

	//STAGE
	Stages eStage;
	int iStageValue;
	void ChangeStrategy(Stages _Stage, int _Value);
	bool bStage;
	bool Stage();
	std::vector<capi::Command> sWaitForOP();
	std::vector<capi::Command> sBuildWell();
	std::vector<capi::Command> sGetUnit();
	std::vector<capi::Command> sSpamBotX();
	std::vector<capi::Command> sFight();
	std::vector<capi::Command> sPanicDef();
	std::vector<capi::Command> sDisablePanicDef();
	std::vector<capi::Command> sTier1();
	std::vector<capi::Command> sTier2();
	std::vector<capi::Command> sDefaultDef();

	
};

/*
auto effect_json = nlohmann::json(state).dump();
std::ofstream file("ALLat" + std::to_string(state.current_tick) + ".json");
file << effect_json << std::endl;
file.close();
*/
