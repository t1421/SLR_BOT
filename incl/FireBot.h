#pragma once

#include "../incl/Broker.h"
#include "../incl/DEBUG.h"
#include "../incl/DataTypes.h"

#include "../API/API/json.hpp"
#include "../API/API/Types.hpp"
#include "../API/API/Helpers.hpp"
#include "../API/API/boost_wrapper.hpp"

#include <thread>
#include <future>
#include <chrono>
using namespace std::chrono_literals;

enum Stages
{
	Defend = 0,
	BuildWell = 1,
	BuildOrb = 2,
	Attack = 3,
	Fight = 4,
	SavePower = 5
};

struct MIS_thread
{
	std::future<std::vector<api::Command>> f;
	std::thread t;
	bool s;

};


struct MIS_AvoidArea
{
	MIS_AvoidArea(api::Tick _endTick, api::Position2D _pos, float _radius) : endTick(_endTick), pos(_pos), radius(_radius) {};
	MIS_AvoidArea(api::Tick _endTick, api::Position2D _pos, float _radius, std::string _note) : endTick(_endTick), pos(_pos), radius(_radius), note(_note) { };
	api::Tick endTick;
	api::Position2D pos;
	float radius;
	std::string note;
};


// /AI: add FireBot FireDeck 4
class FireBot : public IBotImp
{
public:
	FireBot() : IBotImp("FireBot"), myId{}, opId{}, myStart{}, eStage(BuildWell), iSkipTick(0), imyPlayerIDX(0), iopPlayerIDX(0), MaxAvoidID(0){ };
	~FireBot() override = default;
	std::vector<api::Deck> DecksForMap(const api::MapInfo& mapInfo) override;
	void PrepareForBattle(const api::MapInfo& mapInfo, const api::Deck& deck) override;
	void MatchStart(const api::GameStartState& state) override;
	std::vector<api::Command> Tick(const api::GameState& state) override;

	const std::string Name;

	static broker* Bro;
	static void learnBro(broker* _Bro) { Bro = _Bro; }

private:
	std::vector<Card> SMJDeck;
	std::vector<Card> SMJDeckOP;

	api::EntityId myId;
	api::EntityId opId;
	unsigned int imyPlayerIDX;
	unsigned int iopPlayerIDX;
	api::Position2D myStart;

	//unsigned int iStage;
	Stages eStage;
	unsigned int iSkipTick;

	//Instand Functions
	void WellKiller(std::vector<api::Command> vMain, std::vector<api::Entity> Wells);
	void FindAvoidArea(const api::GameState& state);
	void RemoveFromMIS_AvoidArea(api::Tick curTick);
	std::vector<api::Command> MoveUnitsAway(const api::GameState& state);
	std::vector<MIS_AvoidArea *> vAvoid;
	api::EntityId MaxAvoidID;

	//

	int CardPicker(api::CardId opID) { return CardPicker(opID, false); };
	int CardPicker(api::CardId opID, bool Swift);
	int GetSwiftCounterFor(Card OP, bool PerfectCounter, bool Swift);

	std::vector<api::Command> CoolEruption(const api::GameState& state);
	MIS_thread CoolEruptionTest;

	bool Stage(const api::GameState& state);
	
};

/*
auto effect_json = nlohmann::json(state).dump();
std::ofstream file("ALLat" + std::to_string(state.current_tick) + ".json");
file << effect_json << std::endl;
file.close();
*/