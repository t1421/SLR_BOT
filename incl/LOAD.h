#ifndef LOAD_H
#define LOAD_H

//#include "Thread_MIS.h"

#include <string>
#include <vector>



//struct TomeStruct;

class LOAD //: public Thread_MIS
{
public:

	void StartUp();
	void EchoSettings();
	int ProcessArg(int argc, char** argv);

	bool SMJOnline;

	bool WellKiller;
	bool AvoidArea;
	bool UnitEruption;
	bool DrawAvoidArea;
	bool AllTick;
	bool BattleTable;
	bool LowHPMover;
	bool InstantRepair;
	
	unsigned int Port;
	std::string Name;
	std::string Settings;

	std::vector<std::pair<int, int> > vStrategy;	

	//BROKER
	static broker* Bro;
	void teachL() { Bro->L = this; }
	static void learnBro(broker *_Bro) { Bro = _Bro; }

protected:
	
	void Load_Settings();
private:
	std::string entry(std::string Liste, int pos);
	std::string entry(std::string Liste, int pos, std::string delimiter);
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

