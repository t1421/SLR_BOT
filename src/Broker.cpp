//#define DF_Debug
#include "../incl/Broker.h"
#ifdef MIS_DEBUG
#include "../incl/DEBUG.h"
#endif // MIS_DEBUG
#include "../incl/CardBaseSMJ.h"
#include "../incl/Util.h"
#include "../incl/LOAD.h"

broker::broker()
{
	bAktive = true;
	Bro = this;

#ifdef MIS_DEBUG
	DEBUG::learnBro(this);
	B = NULL;
#endif // MIS_DEBUG		

	CardBaseSMJ::learnBro(this);
	J = NULL;

#ifndef MIS_Online

	Util::learnBro(this);
	U = NULL;

	LOAD::learnBro(this);
	L = NULL;
#endif // !MIS_Online
}

#ifdef MIS_DEBUG
void broker::B_StatusNew(std::string Fun, std::string Wert)
{
	B->StatusNew(Fun, Wert);
}

void broker::B_StatusNew(std::string Fun, int Wert)
{
	B->StatusNew(Fun, std::to_string(Wert));
}

void broker::B_StatusE(std::string Modul, std::string Funktion, std::string Wert)
{
	B->StatusE(Modul, Funktion, Wert);
}
#endif // MIS_DEBUG

std::string broker::sTime(unsigned long ulTime)
{
	char cOut[6];
	snprintf(cOut, 6, "%02d:%02d", int(floor(ulTime / 600)), int(ulTime / 10 % 60));
	return cOut;
}

#ifdef MIS_Stream 
void broker::L_StartsPlus()
{
	L->StartsPlus();
}
void broker::L_GamesPlus()
{
	L->GamesPlus();
}
#endif