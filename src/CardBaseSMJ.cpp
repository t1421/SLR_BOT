#define DF_Debug

#include "../incl/Broker.h" 
#define SMJ_Cards "https://smj.cards/api/cards"
#include "../incl/CardBaseSMJ.h" 

#ifdef MIS_Online
#define CURL_STATICLIB
#include <curl/curl.h>

#ifdef _DEBUG
#    pragma comment (lib,"libcurl_a_debug.lib")
#else
#    pragma comment (lib,"libcurl_a.lib")
#endif // _DEBUG
#endif
broker *(CardBaseSMJ::Bro) = NULL;

size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* buffer) 
{
	size_t realsize = size * nmemb;
	buffer->append((char*)contents, realsize);
	return realsize;
}

CardBaseSMJ::CardBaseSMJ()
{
	MISS;
	
	MISE;
}

CardBaseSMJ::~CardBaseSMJ()
{
	MISS;
	
	MISE;
}

#ifdef MIS_Online
bool CardBaseSMJ::DownloadSMJ()
{
	MISS;

	//Bro->B_StatusNew(__FUNCTION__, "on line mode");

	CURL* curl;
	CURLcode res;
	std::string response;

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();

	if (curl) 
	{
		curl_easy_setopt(curl, CURLOPT_URL, SMJ_Cards);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		res = curl_easy_perform(curl);

		if (res != CURLE_OK) 
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			curl_easy_cleanup(curl);
			curl_global_cleanup();
			MISEA("V1")
			return false;
		}

		curl_easy_cleanup(curl);
		nlohmann::json jsonData = nlohmann::json::parse(response);
		std::ofstream file("SMJ.json");
		file << jsonData << std::endl;
		file.close();
				
	}

	curl_global_cleanup();
	//Bro->B_StatusNew(__FUNCTION__, "SMJ.json updated");
	MISE;
	return true;
}
#endif

bool CardBaseSMJ::readJSON()
{
	MISS;
	std::ifstream ifFile;
	ifFile.open("SMJ.json", std::ios::binary);
	if (ifFile.good())
	{
		MISD("good");
		mainJSON = nlohmann::json::parse(ifFile);
		ifFile.clear();
		ifFile.close();
	}
	else
	{
		MISEA("Error opening Jason");
		return false;
	}

	MISE;
	return true;
}

std::string CardBaseSMJ::SwitchAffinity(char _Affinity)
{
	for (unsigned int i = 0; i < EnumAffinity.size(); i++)
		if (EnumAffinity[i].first == std::to_string(_Affinity))return EnumAffinity[i].second;
	
	return "???";
}
std::string CardBaseSMJ::SwitchColor(char _Color)
{
	for (unsigned int i = 0; i < EnumColor.size(); i++)
		if (EnumColor[i].first == std::to_string(_Color))return EnumColor[i].second;

	return "???";
}
std::string CardBaseSMJ::SwitchRarity(char _Rarity)
{
	for (unsigned int i = 0; i < EnumRarity.size(); i++)
		if (EnumRarity[i].first == std::to_string(_Rarity))return EnumRarity[i].second;

	return "???";
}

Card CardBaseSMJ::CardFromJson(unsigned int ID)
{
	MISS;
	if (ID == 4051)ID = 1465; //Test Sirker = Batarial (P)

	for (const auto& item : mainJSON["data"])
	{
		if (item["officialCardIds"].at(0) == ID)
		{
			MISE;
			return Card(item);
		}
	}
	MISERROR("Card not found:" + std::to_string(ID));
	return Card();
}

void CardBaseSMJ::FIX_Cards()
{
	MISS;
	for (auto& item : mainJSON["data"])
	{
		//Firesworn
		if (item["officialCardIds"].at(0) == 1237 || item["officialCardIds"].at(0) == 1386)
		{
			item["offenseType"] = 2; // L Counter
			item["attackType"] = 1; // Melee (to avoid battle vs Flyers)
		}
		//Shadow Mage
		if (item["officialCardIds"].at(0) == 405)
			item["offenseType"] = 1; // M Counter
		//Harvester
		if (item["officialCardIds"].at(0) == 304)
			item["offenseType"] = 2; // L Counter
		//AshbonePyro
		if (item["officialCardIds"].at(0) == 408)
			item["offenseType"] = 2; // L Counter
		//Windweavers
		if (item["officialCardIds"].at(0) == 362)
			item["offenseType"] = 0; // S Counter
		//EnergyParasite
		if (item["officialCardIds"].at(0) == 427)
			item["offenseType"] = 4; // NONE Counter
	}
	MISE;
}