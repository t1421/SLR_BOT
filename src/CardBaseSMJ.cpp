#define DF_Debug

#include "..\incl\Broker.h" 
#define SMJ_Cards "https://smj.cards/api/cards"
#include "..\incl\CardBaseSMJ.h" 

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

	Bro->B_StatusNew(__FUNCTION__, "on line mode");

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
	Bro->B_StatusNew(__FUNCTION__, "SMJ.json updated");
	MISE;
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
