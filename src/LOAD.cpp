//#define DF_Debug

#include "../incl/Broker.h"

#include "../incl/LOAD.h"

#include <fstream>
#include "../CLI/CLI11.hpp"

broker *(LOAD::Bro) = NULL;

void LOAD::Load_Settings()
{
	MISS;
	std::string line;
	int iStrategyCount = 0;
	if(Settings=="") Settings = "Settings.ini";

	std::ifstream ifFile;
	ifFile.open(Settings.c_str(), std::ios::binary);

	if (ifFile.good())
	{
		MISD("good");
		while (getline(ifFile, line))
		{
			line.erase(line.size() - 1);
			//printf("%s\n",line.c_str());

			if (INI_Value_Check(line, "Port") && Port == 0)Port = atoi(line.c_str());
			if (INI_Value_Check(line, "Name") && Name == "")Name = line.c_str();

			if (INI_Value_Check(line, "SMJOnline"))SMJOnline = line.substr(0,1) != "0";
			if (INI_Value_Check(line, "WellKiller"))WellKiller = line.substr(0, 1) != "0";			
			if (INI_Value_Check(line, "AvoidArea"))AvoidArea = line.substr(0, 1) != "0";			
			if (INI_Value_Check(line, "LowHPMover"))LowHPMover = line.substr(0, 1) != "0";			
			if (INI_Value_Check(line, "TragetSwitcher"))TragetSwitcher = line.substr(0, 1) != "0";
			if (INI_Value_Check(line, "ResetUnitsOnStratSwitch"))ResetUnitsOnStratSwitch = line.substr(0, 1) != "0";			
			if (INI_Value_Check(line, "IgnoreEmptyWalls"))IgnoreEmptyWalls = line.substr(0, 1) != "0";			

			if (INI_Value_Check(line, "UnitEruption"))UnitEruption = line.substr(0, 1) != "0";
			if (INI_Value_Check(line, "BattleTable"))BattleTable = line.substr(0, 1) != "0";
			if (INI_Value_Check(line, "InstantRepair"))InstantRepair = line.substr(0, 1) != "0";			
			
			if (INI_Value_Check(line, "DrawAvoidArea"))DrawAvoidArea = line.substr(0, 1) != "0";
			if (INI_Value_Check(line, "DrawRejected"))DrawRejected = line.substr(0, 1) != "0";
			if (INI_Value_Check(line, "DrawSpawn"))DrawSpawn = line.substr(0, 1) != "0";
			if (INI_Value_Check(line, "EchoRejected"))EchoRejected = line.substr(0, 1) != "0";			
			if (INI_Value_Check(line, "AllTick"))AllTick = line.substr(0, 1) != "0";


			if (INI_Value_Check(line, "CastRange"))CastRange = atoi(line.c_str());
			if (INI_Value_Check(line, "FightRange"))FightRange = atoi(line.c_str());
			if (INI_Value_Check(line, "RetreatRange"))RetreatRange = atoi(line.c_str());
			if (INI_Value_Check(line, "SwitchToDefRange"))SwitchToDefRange = atoi(line.c_str());
			if (INI_Value_Check(line, "HealRange"))HealRange = atoi(line.c_str());
			if (INI_Value_Check(line, "SaveRangeWellOrb"))SaveRangeWellOrb = atoi(line.c_str());
			if (INI_Value_Check(line, "SwitchTargetRange"))SwitchTargetRange = atoi(line.c_str());
			if (INI_Value_Check(line, "WallSearchRange"))WallSearchRange = atoi(line.c_str());
			

			if (INI_Value_Check(line, "WaitSpawnTime"))WaitSpawnTime = atoi(line.c_str());
			if (INI_Value_Check(line, "OrbBuildWait"))OrbBuildWait = atoi(line.c_str());
			if (INI_Value_Check(line, "TierCheckOffset"))TierCheckOffset = atoi(line.c_str());
			if (INI_Value_Check(line, "WellCheckOffset"))WellCheckOffset = atoi(line.c_str());
			if (INI_Value_Check(line, "Tier2Init"))Tier2Init = atoi(line.c_str());
			if (INI_Value_Check(line, "Tier3Init"))Tier3Init = atoi(line.c_str());

			if (INI_Value_Check(line, "ToMutchEnergy"))ToMutchEnergy = atoi(line.c_str());

			if (INI_Value_Check(line, "GiveUpFight"))GiveUpFight = atoi(line.c_str());
			if (INI_Value_Check(line, "AddSiegeToMix"))AddSiegeToMix = atoi(line.c_str());
			if (INI_Value_Check(line, "MoreUnitsGlobal"))MoreUnitsGlobal = atoi(line.c_str());
			if (INI_Value_Check(line, "MoreUnitsPerType"))MoreUnitsPerType = atoi(line.c_str());
			
			ifFile.clear();
		}
		
		ifFile.close();
	}
	else
	{
		MISERROR(Settings + " not found")
	}

	//Fall back
	if (Port == 0)Port = 6370;
	if (Name == "")Name = "FireBot";
	MISE;
}

void LOAD::StartUp()
{
	MISS;
	Load_Settings();
	MISE;
}

std::string LOAD::entry(std::string Liste, int pos)
{
	return entry(Liste, pos, ";");
}

std::string LOAD::entry(std::string Liste, int pos, std::string delimiter)
{
	if (pos == 0)return Liste.substr(0, Liste.find(delimiter));
	else
	{
		Liste.erase(0, Liste.find(delimiter) + 1);
		return entry(Liste, pos - 1, delimiter);
	}
}

bool LOAD::INI_Value_Check(std::string& check, std::string name)
{
	if (check.substr(0, check.find("=")) == name)
	{
		check.erase(0, check.find("=") + 1);
		return true;
	}
	else
	{
		return false;
	}
}


void LOAD::EchoSettings()
{
	MISS;
	MISERROR("########################");
	MISERROR("### Current Settings ###");
	MISERROR("########################");
	MISERROR("# use --help for options");
	MISERROR("########################");
	MISERROR(("# Port         = " + std::to_string(Port)).c_str());
	MISERROR(("# Name         = " + Name).c_str());
	MISERROR("########################");
	MISERROR(("# SMJOnline    = " + std::to_string(SMJOnline)).c_str());
	MISERROR("########################");
	MISERROR(("# WellKiller   = " + std::to_string(WellKiller)).c_str());	
	MISERROR(("# AvoidArea    = " + std::to_string(AvoidArea)).c_str());	
	MISERROR(("# Trag.Switcher= " + std::to_string(TragetSwitcher)).c_str());
	MISERROR(("# ResetUnits   = " + std::to_string(ResetUnitsOnStratSwitch)).c_str());	
	MISERROR(("# IgnoreEmptyWa= " + std::to_string(IgnoreEmptyWalls)).c_str());
	
	MISERROR("########################");
	MISERROR(("# UnitEruption = " + std::to_string(UnitEruption)).c_str());
	MISERROR(("# BattleTable  = " + std::to_string(BattleTable)).c_str());
	MISERROR(("# InstantRepair= " + std::to_string(InstantRepair)).c_str());
	MISERROR("########################");
	MISERROR("# RANGES");
	MISERROR("########################");
	MISERROR(("# CastRange    = " + std::to_string(CastRange)).c_str());
	MISERROR(("# FightRange   = " + std::to_string(FightRange)).c_str());
	MISERROR(("# RetreatRange = " + std::to_string(RetreatRange)).c_str());
	MISERROR(("# SwitchDefRang= " + std::to_string(SwitchToDefRange)).c_str());
	MISERROR(("# HealRange    = " + std::to_string(HealRange)).c_str());
	MISERROR(("# SaveRangeWell= " + std::to_string(SaveRangeWellOrb)).c_str());
	MISERROR(("# SwitchTargRan= " + std::to_string(SwitchTargetRange)).c_str());
	MISERROR(("# WallSearchRan= " + std::to_string(WallSearchRange)).c_str());
	
	MISERROR("########################");
	MISERROR("# Timings");
	MISERROR("########################");
	MISERROR(("# WaitSpawnTime= " + std::to_string(WaitSpawnTime)).c_str());
	MISERROR(("# OrbBuildWait = " + std::to_string(OrbBuildWait)).c_str());
	MISERROR(("# TierCheckOffs= " + std::to_string(TierCheckOffset)).c_str());
	MISERROR(("# WellCheckOffs= " + std::to_string(WellCheckOffset)).c_str());
	MISERROR(("# Tier2Init    = " + std::to_string(Tier2Init)).c_str());
	MISERROR(("# Tier3Init    = " + std::to_string(Tier3Init)).c_str());
	MISERROR("########################");
	MISERROR("# Power Levels");
	MISERROR("########################");
	MISERROR(("# GiveUpFight  = " + std::to_string(GiveUpFight)).c_str());
	MISERROR(("# AddSiegeToMix= " + std::to_string(AddSiegeToMix)).c_str());	
	MISERROR(("# MoreUnitsGlob= " + std::to_string(MoreUnitsGlobal)).c_str());
	MISERROR(("# MoreUnitsType= " + std::to_string(MoreUnitsPerType)).c_str());
	MISERROR("########################");
	MISERROR("# Energy Pools");
	MISERROR("########################");
	MISERROR(("# ToMutchEnergy= " + std::to_string(ToMutchEnergy)).c_str());	
	MISERROR("########################");	
	MISERROR(("# DrawAvoidArea= " + std::to_string(DrawAvoidArea)).c_str());
	MISERROR(("# DrawRejected = " + std::to_string(DrawRejected)).c_str());
	MISERROR(("# DrawSpawn    = " + std::to_string(DrawSpawn)).c_str());
	MISERROR(("# EchoRejected = " + std::to_string(EchoRejected)).c_str());
	MISERROR(("# AllTick      = " + std::to_string(AllTick)).c_str());
	MISERROR("########################");
	MISE;
}

int LOAD::ProcessArg(int argc, char** argv)
{
	MISS;
	CLI::App app{ "MIS BOT" };

	app.add_option("--p", Port, "port (default 6370)");
	app.add_option("--n", Name, "name of bot (default FireBot)");
	app.add_option("--s", Settings, "settings file (default settings.ini)");

	CLI11_PARSE(app, argc, argv);

	MISE;
	if (Name != "")return 1;
	return 0;
}


#ifdef MIS_Stream 
void LOAD::Load_Stream()
{
	MISS;
	std::string line;
	std::string sName;
	std::ifstream ifFile;

	sName = "Stream_Starts.ini";
	ifFile.open(sName.c_str(), std::ios::binary);
	if (ifFile.good())
	{
		getline(ifFile, line);
		iStarts = atoi(line.c_str());
		ifFile.clear();
		ifFile.close();
	}

	sName = "Stream_Games.ini";
	ifFile.open(sName.c_str(), std::ios::binary);
	if (ifFile.good())
	{
		getline(ifFile, line);
		iGames = atoi(line.c_str());
		ifFile.clear();
		ifFile.close();
	}

	MISE;
}

void LOAD::Save_Stream()
{
	MISS;

	std::string sName;
	std::ofstream ofFile;

	sName = "K:\\c++\\SLR_BOT\\Stream_Starts.ini";
	ofFile.open(sName.c_str(), std::ios::binary);
	if (ofFile.good())
	{
		ofFile << iStarts;
		ofFile.close();
	}

	sName = "K:\\c++\\SLR_BOT\\Stream_Games.ini";
	ofFile.open(sName.c_str(), std::ios::binary);
	if (ofFile.good())
	{
		ofFile << iGames;
		ofFile.close();
	}
	MISE;
}
#endif