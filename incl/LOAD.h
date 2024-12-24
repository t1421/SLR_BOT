#ifndef LOAD_H
#define LOAD_H

#include <string>
#include <vector>


class LOAD //: public Thread_MIS
{
public:

	void StartUp();
	void EchoSettings();
	int ProcessArg(int argc, char** argv);

	std::string entry(std::string Liste, int pos);
	std::string entry(std::string Liste, int pos, std::string delimiter);

	//Functions
	bool SMJOnline;
	bool WellKiller;
	bool AvoidArea;	
	bool LowHPMover;
	bool TragetSwitcher;
	bool ResetUnitsOnStratSwitch;
	bool IgnoreEmptyWalls;	
	bool UnitEruption;
	bool BattleTable;
	bool InstantRepair;
	bool DistancTypeTwo;

	//Ranges
	int CastRange;
	int FightRange;
	int RetreatRange;
	int SwitchToDefRange;
	int HealRange;
	int SaveRangeWellOrb;
	int SwitchTargetRange;
	int WallSearchRange;
	int DefRange;
	int PanicDefSearch;

	//Timings
	int WaitSpawnTime;
	int OrbBuildWait;
	int TierCheckOffset;
	int Tier2Init;
	int WellCheckOffset;
	int Tier3Init;
	int IdleOffset;

	//Interval
	int InstaRepairInterval;
	int BattleTableInterval;
	int UpdateBasesInterval;
	int UnitEruptionInterval;
	int WellKillerInterval;
	int StrategyInterval;
	int FixType2Interval;
	int CleanSaveUnitInterval;

	//Power
	int GiveUpFight;
	int AddSiegeToMix;
	int MoreUnitsGlobal;
	int MoreUnitsPerType;
	
	//Energy
	int ToMutchEnergy;
	

	//DEBUGs
	bool DrawAvoidArea;
	bool DrawRejected;
	bool DrawSpawn;
	bool EchoRejected;
	bool AllTick;
	
	unsigned int Port;
	std::string Name;
	std::string Settings;

	//BROKER
	static broker* Bro;
	void teachL() { Bro->L = this; }
	static void learnBro(broker *_Bro) { Bro = _Bro; }

private:
	void Load_Settings();
	bool INI_Value_Check(std::string& check, std::string name);


#ifdef MIS_Stream 
private:
	unsigned int iStarts;
	unsigned int iGames;
public:
	void Load_Stream();
	void Save_Stream();
	void StartsPlus() {iStarts++; Save_Stream();};
	void GamesPlus() { iGames++; Save_Stream(); };

#endif
};

#endif //LOAD_H

