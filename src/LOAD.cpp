//#define DF_Debug

#include "..\incl\Broker.h"

#include "..\incl\LOAD.h"

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
			if (INI_Value_Check(line, "StartType"))StartType = atoi(line.c_str());
			
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
	Bro->B_StatusNew("", "########################");
	Bro->B_StatusNew("", "### Current Settings ###");
	Bro->B_StatusNew("", "########################");
	Bro->B_StatusNew("", "# SMJOnline    = " + std::to_string(SMJOnline));
	Bro->B_StatusNew("", "########################");
	Bro->B_StatusNew("", "# WellKiller   = " + std::to_string(WellKiller));
	Bro->B_StatusNew("", "# UnitEruption = " + std::to_string(UnitEruption));
	Bro->B_StatusNew("", "# StartType    = " + std::to_string(StartType));
	Bro->B_StatusNew("", "########################");
	MISE;
}