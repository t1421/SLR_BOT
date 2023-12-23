#pragma once
//#define DF_Debug

#define CastRange 14.5

	#ifdef DF_Debug 
		#define MISD(___Mes___) Bro->B_StatusNew(__FUNCTION__, ___Mes___);
		#define MISERROR(___Mes___)Bro->B_StatusE("E", __FUNCTION__, ___Mes___);
		#define MISS MISD("-->");
		#define MISE MISD("<--");
		#define MISEA(___Mes___) MISD("<-- " ___Mes___);
	#else
		#define MISD(___Mes___); 
		#define MISERROR(___Mes___)Bro->B_StatusE("E", __FUNCTION__, ___Mes___);
		#define MISS
		#define MISE
		#define MISEA(___Mes___);
	#endif


#include <string>

class DEBUG;
class CardBaseSMJ;
class Util;
class LOAD;

class broker
{
public:
	broker();

	DEBUG* B;
	Util* U;
	CardBaseSMJ* J;
	LOAD* L;
	
	broker* Bro;

	bool bAktive;
	
	void B_StatusNew(std::string Fun, std::string Wert);
	void B_StatusNew(std::string Fun, int Wert);
	void B_StatusE(std::string Modul, std::string Funktion, std::string Wert);

	std::string sTime(unsigned long ulTime);

#ifdef MIS_Stream 
	void L_StartsPlus();
	void L_GamesPlus();
#endif


protected:
private:
};

