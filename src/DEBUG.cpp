#define Log_path "./log/"

#include "../incl/DEBUG.h"

#include "../API/API/json.hpp"
#include "../API/API/Types.hpp"
#include "../API/API/Helpers.hpp"

//#include "../API/API/boost_wrapper.hpp"

broker *(DEBUG::Bro) = NULL;

DEBUG::DEBUG(std::string sLogName, bool _bGUI, bool _bFile, bool _bFilter)
{
	bGUI = _bGUI;
	bFile = _bFile;
	bFilter = _bFilter;


	printf("DEBUG WIRD GESTARTET -->\n");
	Fill_DBArray("*", "*", "*", "true");
		

	/////////////////////////////////////////////////////
	///Fill_DBArray("DEBUG", "*", "*", "true");
	/////////////////////////////////////////////////////
	Fill_DBArray("E", "*", "*", "true"); //ERRORS

	Fill_DBArray("", "*", "*", "true"); // MAIN

	Fill_DBArray("FireBot", "*", "*", "true");		
	Fill_DBArray("FireBot", "CloseCombi", "*", "false");
	Fill_DBArray("FireBot", "UpdateStuff", "*", "false");

	Fill_DBArray("CardBaseSMJ", "*", "*", "true");
	
	
	//Fill_DBArray("broker", "*", "*", "true");
	/////////////////////////////////////////////////////

	if (bFile)ofB.open(Log_path + sLogName + ".log", std::ios::binary);
	ofE.open(Log_path + sLogName + "_E.log", std::ios::binary | std::ofstream::app);

	printf("DEBUG WIRD GESTARTET <--\n");
}


void DEBUG::Fill_DBArray(std::string Modul, std::string Funktion, std::string Wert, std::string Status)
{
	std::vector<std::string> vs;
	DebugInfo.push_back(vs);

	DebugInfo[DebugInfo.size() - 1].push_back(Modul);
	DebugInfo[DebugInfo.size() - 1].push_back(Funktion);
	DebugInfo[DebugInfo.size() - 1].push_back(Wert);
	DebugInfo[DebugInfo.size() - 1].push_back(Status);
}

void DEBUG::StatusE(std::string Modul, std::string Funktion, std::string Wert)
{
	ofE << Modul << ";" << Funktion << ";" << Wert << std::endl;
	StatusNew(Modul + "::" + Funktion, Wert);
}

void DEBUG::StatusNew(std::string Fun, std::string Wert)
{
	std::string sClass = "";
	std::string sMethode;

	size_t colons = Fun.find("::");

	if (colons != std::string::npos)
	{
		sMethode = Fun.substr(colons + 2, Fun.rfind("("));		
		if(Fun.find(" ")!= std::string::npos)
			sClass = Fun.substr(Fun.find(" ") + 1, colons - Fun.find(" ") - 1);
		else sClass = Fun.substr(0,colons);
	}
	else sMethode = Fun.substr(0, Fun.rfind("("));

	if ( bGUI == false &&
		bFile == false)return;

	t = time(NULL);	
	localtime_s(&ts, &t);

	if (bFilter && (Wert == "-->" || Wert == "<--"))return;

	if(bGUI || bFile)
		if (check_MFW(sClass, sMethode, Wert))
		{
			mtx.lock();
			if (bGUI)
			{
				printf("%02i:%02i:%02i - %-20.20s %-20.20s %-60.60s\n", ts.tm_hour, ts.tm_min, ts.tm_sec,
					sClass.c_str(), sMethode.c_str(), Wert.c_str());
			}
			if (bFile)
			{
				ofB << ts.tm_hour << ":" << ts.tm_min << ":" << ts.tm_sec << ";";
				ofB << sClass.c_str() << ";" << sMethode.c_str() << ";" << Wert.c_str() << std::endl;
			}
			mtx.unlock();
		}
}

bool DEBUG::check_MFW(std::string Modul, std::string Funktion, std::string Wert)
{
	std::string return_ = "";

	for (unsigned int i = 0; i < DebugInfo.size(); i++)
	{
		if (DebugInfo[i][0] == Modul &&
			DebugInfo[i][1] == "*" &&
			DebugInfo[i][2] == "*")return_ = DebugInfo[i][3];

		if (DebugInfo[i][0] == Modul &&
			DebugInfo[i][1] == Funktion &&
			DebugInfo[i][2] == "*")return_ = DebugInfo[i][3];

		if (DebugInfo[i][0] == Modul &&
			DebugInfo[i][1] == Funktion &&
			DebugInfo[i][2] == Wert)return_ = DebugInfo[i][3];
	}

	if (return_ == "true")return true;

	return false;
}



std::string DEBUG::switchAPIEntitySpecific(api::APIEntitySpecific& v) 
{
	switch (v.v.index()) 
	{
	case 0: return "Projectile"; break;
	case 1: return "PowerSlot"; break;
	case 2: return "TokenSlot"; break;
	case 3: return "AbilityWorldObject"; break;
	case 4: return "Squad"; break;
	case 5: return "Figure"; break;
	case 6: return "Building"; break;
	case 7: return "BarrierSet"; break;
	case 8: return "BarrierModule"; break;
	default: return "invalid case";
	}
}

std::string DEBUG::switchCommandRejectionReason(api::CommandRejectionReason& v) 
{
	switch (v.v.index()) 
	{
	case 0: return "Other"; break;
	case 1: return "NotEnoughPower"; break;
	case 2: return "SpellDoesNotExist"; break;
	case 3: return "EntityDoesNotExist"; break;
	case 4: return "InvalidEntityType"; break;
	case 5: return "CanNotCast"; break;
	case 6: return "EntityNotOwned"; break;
	case 7: return "EntityOwnedBySomeoneElse"; break;
	case 8: return "NoModeChange"; break;
	case 9: return "EntityAlreadyInThisMode"; break;
	case 10: return "ModeNotExist"; break;
	case 11: return "InvalidCardIndex"; break;
	case 12: return "InvalidCard"; break;
	default: return "invalid case";
	}
}

std::string DEBUG::switchAPICommand(api::APICommand& v) 
{
	switch (v.v.index()) 
	{
	case 0: return "BuildHouse"; break;
	case 1: return "CastSpellGod"; break;
	case 2: return "CastSpellGodMulti"; break;
	case 3: return "ProduceSquad"; break;
	case 4: return "ProduceSquadOnBarrier"; break;
	case 5: return "CastSpellEntity"; break;
	case 6: return "BarrierGateToggle"; break;
	case 7: return "BarrierBuild"; break;
	case 8: return "BarrierRepair"; break;
	case 9: return "BarrierCancelRepair"; break;
	case 10: return "RepairBuilding"; break;
	case 11: return "CancelRepairBuilding"; break;
	case 12: return "GroupAttack"; break;
	case 13: return "GroupEnterWall"; break;
	case 14: return "GroupExitWall"; break;
	case 15: return "GroupGoto"; break;
	case 16: return "GroupHoldPosition"; break;
	case 17: return "GroupStopJob"; break;
	case 18: return "ModeChange"; break;
	case 19: return "PowerSlotBuild"; break;
	case 20: return "TokenSlotBuild"; break;
	case 21: return "Ping"; break;
	case 22: return "Surrender"; break;
	case 23: return "WhisperToMaster"; break;
	default: return "invalid case";
	}
}