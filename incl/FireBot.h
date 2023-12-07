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

	void UpdateStuff(const api::APIGameState& state);

	api::EntityId myId;
	api::EntityId opId;

	uint8_t myTeamId;
	std::vector<api::EntityId> oponents;
	api::Position2D myStart;

	unsigned int iPlayerCount;
	unsigned int imyPlayerIDX;
	bool once;

	std::vector<api::APIEntity> MyBuildings;
	std::vector<api::APIEntity> MyUnits;
	std::vector<api::APIEntity> OpUnits;
	std::vector<api::APIEntity> FreeWells;
	std::vector<api::APIEntity> OpWells;
	std::vector<api::APIEntity> FreeOrbs;
	

	unsigned int iStage;
};