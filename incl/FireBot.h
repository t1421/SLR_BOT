#pragma once

#include "../incl/Broker.h"
#include "../incl/DEBUG.h"

#include "../API/API/json.hpp"
#include "../API/API/Types.hpp"
#include "../API/API/Helpers.hpp"
#include "../API/API/boost_wrapper.hpp"

using json = nlohmann::json;



class FireBot : public IBotImp
{
public:
	FireBot() : IBotImp("FireBot"), myId{}, myTeamId{}, oponents{}, myStart{} , iPlayerCount(9), once{true}, iStage(0)
	{ 
		Bro->B_StatusNew("", "???");
	}
	~FireBot() override = default;
	std::vector<api::DeckAPI> DecksForMap(const api::MapInfo& mapInfo) override;
	void PrepareForBattle(const api::MapInfo& mapInfo, const api::DeckAPI& deck) override;
	void MatchStart(const api::APIGameStartState& state) override;
	std::vector<api::APICommand> Tick(const api::APIGameState& state) override;

	const std::string Name;

	static broker* Bro;
	static void learnBro(broker* _Bro) { Bro = _Bro; }

private:
	float CloseCombi(std::vector<api::APIEntity> EntitiesA, std::vector<api::APIEntity> EntitiesB, unsigned int& outA, unsigned int& outB);
	void UpdateStuff(const api::APIGameState& state);

	api::EntityId myId;
	api::EntityId opId;

	uint8_t myTeamId;
	std::vector<api::EntityId> oponents;
	api::Position2D myStart;

	std::vector<api::APIEntity> Wells;
	std::vector<api::APIEntity> Orbs;

	unsigned int iPlayerCount;
	unsigned int imyPlayerIDX;
	bool once;

	std::vector<api::APIEntity> MyBuildings;
	std::vector<api::APIEntity> MyUnits;
	std::vector<api::APIEntity> FreeWells;
	std::vector<api::APIEntity> FreeOrbs;
	std::vector<api::APIEntity> OpWells;

	unsigned int iStage;
};