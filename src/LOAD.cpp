//#define DF_Debug

#include "../incl/Broker.h"

#include "../incl/LOAD.h"

#include <fstream>

broker *(LOAD::Bro) = NULL;

void LOAD::Load_Settings()
{
	MISS;
	std::string line;
	std::string sName = "Settings.ini";

	std::ifstream ifFile;
	ifFile.open(sName.c_str(), std::ios::binary);

	if (ifFile.good())
	{
		MISD("good");
		while (getline(ifFile, line))
		{
			line.erase(line.size() - 1);
			//printf("%s\n",line.c_str());

			if (INI_Value_Check(line, "SMJOnline"))SMJOnline = line.substr(0,1) != "0";
			if (INI_Value_Check(line, "WellKiller"))WellKiller = line.substr(0, 1) != "0";
			if (INI_Value_Check(line, "UnitEruption"))UnitEruption = line.substr(0, 1) != "0";
			if (INI_Value_Check(line, "AvoidArea"))AvoidArea = line.substr(0, 1) != "0";
			if (INI_Value_Check(line, "BattleTable"))BattleTable = line.substr(0, 1) != "0";
			
			if (INI_Value_Check(line, "StartType"))StartType = atoi(line.c_str());
			if (INI_Value_Check(line, "Port"))Port = atoi(line.c_str());
			if (INI_Value_Check(line, "Name"))Name = line.c_str();
			
			if (INI_Value_Check(line, "DrawAvoidArea"))DrawAvoidArea = line.substr(0, 1) != "0";
			if (INI_Value_Check(line, "AllTick"))AllTick = line.substr(0, 1) != "0";
			
			
			ifFile.clear();
		}
		
		ifFile.close();
	}
	MISE;
}

void LOAD::StartUp()
{
	MISS;
	Load_Settings();

	if (StartType == 1 && !BattleTable)
	{
		MISERROR("StartType = 1 needs BattleTable activ -> switched: StartType = 2");
		StartType = 2;
	}

	MISE;
}

std::string LOAD::entry(std::string Liste, int pos)
{
	if (pos == 0)return Liste.substr(0, Liste.find(";"));
	else
	{
		Liste.erase(0, Liste.find(";") + 1);
		return entry(Liste, pos - 1);
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
	MISERROR("# Port         = " + std::to_string(Port));
	MISERROR("# Name         = " + Name);
	MISERROR("# Use Settings.ini");
	MISERROR("# Bot.exe [Port] [Name]");
	MISERROR("########################");
	MISERROR("# SMJOnline    = " + std::to_string(SMJOnline));
	MISERROR("########################");
	MISERROR("# WellKiller   = " + std::to_string(WellKiller));
	MISERROR("# UnitEruption = " + std::to_string(UnitEruption));
	MISERROR("# AvoidArea    = " + std::to_string(AvoidArea));
	MISERROR("# BattleTable  = " + std::to_string(BattleTable));
	MISERROR("########################");
	MISERROR("# StartType    = " + std::to_string(StartType));
	MISERROR("########################");
	MISERROR("# DrawAvoidArea= " + std::to_string(DrawAvoidArea));	
	MISERROR("# AllTick      = " + std::to_string(AllTick));
	MISERROR("########################");
	MISE;
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