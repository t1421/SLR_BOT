#ifndef CardBaseSMJ_H
#define CardBaseSMJ_H

#include <fstream>
#include <vector>

#ifdef SMJonline

#endif // SMJonline
//#include <nlohmann/json.hpp>
#include "../API/API/json.hpp"

//#include "DataTypes.h" 

class CardBaseSMJ
{
public:
	CardBaseSMJ();
	~CardBaseSMJ();
	
	std::vector<std::pair<std::string, std::string>> EnumColor;
	std::vector<std::pair<std::string, std::string>> EnumRarity;
	std::vector<std::pair<std::string, std::string>> EnumAffinity;
	std::vector<std::pair<std::string, std::string>> EnumType;
	std::vector<std::pair<std::string, std::string>> EnumBoosters;
	
public:
	
#ifdef MIS_Online
	bool DownloadSMJ();
#else
	bool DownloadSMJ() { printf("SMJ: off line mode\n"); return false; };
#endif

	bool readJSON();
	nlohmann::json mainJSON;

	std::string SwitchAffinity(char _Affinity);
	std::string SwitchColor(char _Color);
	std::string SwitchRarity(char _Rarity);
	//SMJCard* GetSMJCard(unsigned short _CardID);
	
	//std::vector<SMJCard*> SMJMatrix;	
	//bool Init();

	//BROKER
	static broker* Bro;
	void teachJ() { Bro->J = this; }
	static void learnBro(broker *_Bro) { Bro = _Bro; }

protected:
private:

};

#endif //CardBase_H


