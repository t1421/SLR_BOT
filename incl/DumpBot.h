#pragma once

#include "../incl/Broker.h"
#include "../incl/DEBUG.h"

#include "../API/API/json.hpp"
#include "../API/API/TypesCLike.hpp"
#include "../API/API/HelpersCLike.hpp"

#include "../API/API/boost_wrapper.hpp"
/*
#include <future>
#include <chrono>
#include <string>
using namespace std::chrono_literals;
*/

// /AI: add FireBot FireDeck 4
class DumpBot : public capi::IBotImp
{
public:
	DumpBot(std::string sBotNme) : capi::IBotImp(sBotNme){ };
	~DumpBot() override = default;
	std::vector<capi::Deck> DecksForMap(const capi::MapInfo& mapInfo) override;
	void PrepareForBattle(const capi::MapInfo& mapInfo, const capi::Deck& deck) override;
	void MatchStart(const capi::GameStartState& state) override;
	capi::TickResponse Tick(const capi::GameState& state) override;

	//const std::string Name;

	static broker* Bro;
	static void learnBro(broker* _Bro) { Bro = _Bro; }

private:
	/*
	capi::GameState lState;
	capi::EntityId myId;
	capi::EntityId opId;
	capi::EntityId MaxAvoidID;
	capi::Entity eMainOrb;
	capi::MapInfo mapinfo;

	std::vector<capi::EntityId> vSaveUnit;
	std::vector<MIS_Ideler *> vIdler;
	std::vector<MIS_RejectCheck> RejectedComamandChecklist;
	std::vector<BaseDef *> vBases;
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

	//Base FUnktion
	void UpdateBases();

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

	*/
};
