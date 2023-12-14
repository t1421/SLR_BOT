//#define DF_Debug
#include "../incl/Broker.h"
#include "../incl/DEBUG.h"
#include "../incl/CardBaseSMJ.h"

broker::broker()
{
	bAktive = true;
	Bro = this;

	DEBUG::learnBro(this);
	B = NULL;	

	CardBaseSMJ::learnBro(this);
	J = NULL;
}

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

std::string broker::sTime(unsigned long ulTime)
{
	char cOut[6];
	snprintf(cOut, 6, "%02d:%02d", int(floor(ulTime / 600)), int(ulTime / 10 % 60));
	return cOut;
}
