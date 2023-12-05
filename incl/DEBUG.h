#pragma once
#include "../incl/Broker.h"


#include <string>
#include <mutex> 
#include <vector>
#include <fstream>

namespace api 
{
	struct APIEntitySpecific;
	struct CommandRejectionReason;
	struct APICommand;
}
class DEBUG
{
public:
	bool bGUI;
	bool bFile;
	bool bFilter;

	DEBUG(std::string sLogName, bool _bGUI, bool _bFile, bool _bFilter);
	
	void StatusE(std::string Modul, std::string Funktion, std::string Wert);
	void StatusNew(std::string Fun, std::string Wert);
	bool check_MFW(std::string Modul, std::string Funktion, std::string Wert);
	void Fill_DBArray(std::string Modul, std::string Funktion, std::string Wert, std::string Status);
	
	std::vector<std::vector<std::string>> DebugInfo;
	
    time_t t;
    struct tm ts ;

	std::ofstream ofB;
	std::ofstream ofE;

	std::mutex mtx;

	/////////////////
	std::string switchAPIEntitySpecific(api::APIEntitySpecific& v);
	std::string switchCommandRejectionReason(api::CommandRejectionReason& v);
	std::string switchAPICommand(api::APICommand& v);
	/////////////////

	static broker* Bro;
	void teachB() { Bro->B = this; }
	static void learnBro(broker *_Bro) { Bro = _Bro; }	

protected:

	
private:

};
