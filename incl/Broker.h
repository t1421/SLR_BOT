#pragma once
//#define DF_Debug

#define CastRange 14.5

#ifdef MIS_DEBUG
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
#else
		#define MISD(___Mes___); 
		#define MISERROR(___Mes___)printf("%s # %s\n",__FUNCTION__, ___Mes___);
		#define MISS
		#define MISE
		#define MISEA(___Mes___);
#endif // MIS_DEBUG


#include <string>

#ifdef MIS_DEBUG
class DEBUG;
#endif // MIS_DEBUG

class CardBaseSMJ;
class Util;
class LOAD;

class broker
{
public:
	broker();

#ifdef MIS_DEBUG
	DEBUG* B;
#endif // MIS_DEBUG	
	Util* U;
	CardBaseSMJ* J;
	LOAD* L;
	
	broker* Bro;

	bool bAktive;
	
#ifdef MIS_DEBUG
	void B_StatusNew(std::string Fun, std::string Wert);
	void B_StatusNew(std::string Fun, int Wert);
	void B_StatusE(std::string Modul, std::string Funktion, std::string Wert);
	std::string sComand;
#endif // MIS_DEBUG	

	std::string sTime(unsigned long ulTime);

#ifdef MIS_Stream 
	void L_StartsPlus();
	void L_GamesPlus();
#endif


protected:
private:
};

