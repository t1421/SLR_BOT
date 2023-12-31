#ifndef LOAD_H
#define LOAD_H

//#include "Thread_MIS.h"

#include <string>
#include <vector>

struct TomeStruct;

class LOAD //: public Thread_MIS
{
public:

	void StartUp();
	void EchoSettings();

	bool SMJOnline;

	bool WellKiller;
	bool UnitEruption;
	unsigned int StartType;

	//BROKER
	static broker* Bro;
	void teachL() { Bro->L = this; }
	static void learnBro(broker *_Bro) { Bro = _Bro; }

protected:
	
	void Load_Settings();
private:
	std::string entry(std::string Liste, int pos);
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

