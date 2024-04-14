#pragma once

#include "../incl/Broker.h"
#include "../incl/DEBUG.h"
#include "../incl/DataTypes.h"

//#include "../API/API/Types.hpp"
//#include "../API/API/Helpers.hpp"

#include "../API/API/json.hpp"
#include "../API/API/TypesCLike.hpp"
#include "../API/API/HelpersCLike.hpp"

#include "../API/API/boost_wrapper.hpp"

//#include <thread>
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
	std::future<std::vector<capi::Command>> fc;
	std::future<bool> fb;
	//std::thread t;
	bool s;

};


struct MIS_AvoidArea
{
	MIS_AvoidArea(capi::Tick _endTick, capi::Position2D _pos, float _radius) : endTick(_endTick), pos(_pos), radius(_radius) {};
	MIS_AvoidArea(capi::Tick _endTick, capi::Position2D _pos, float _radius, std::string _note) : endTick(_endTick), pos(_pos), radius(_radius), note(_note) { };
	capi::Tick endTick;
	capi::Position2D pos;
	float radius;
	std::string note;
};


// /AI: add FireBot FireDeck 4
class FireBot : public capi::IBotImp
{
public:
	FireBot() : capi::IBotImp("FireBot"), myId{}, opId{}, myStart{}, eStage(BuildWell), iSkipTick(0), imyPlayerIDX(0), iopPlayerIDX(0), MaxAvoidID(0){ };
	~FireBot() override = default;
	std::vector<capi::Deck> DecksForMap(const capi::MapInfo& mapInfo) override;
	void PrepareForBattle(const capi::MapInfo& mapInfo, const capi::Deck& deck) override;
	void MatchStart(const capi::GameStartState& state) override;
	std::vector<capi::Command> Tick(const capi::GameState& state) override;

	const std::string Name;

	static broker* Bro;
	static void learnBro(broker* _Bro) { Bro = _Bro; }

private:
	std::vector<Card> SMJDeck;
	std::vector<Card> SMJDeckOP;
	capi::EntityId myId;
	capi::EntityId opId;
	unsigned int imyPlayerIDX;
	unsigned int iopPlayerIDX;
	BattleTable myBT;
	BattleTable opBT;
	capi::Position2D myStart;


	Stages eStage;
	unsigned int iSkipTick;

	//Instand Functions
	bool WellKiller(std::vector<capi::Command> vMain, std::vector<capi::Entity> Wells);
	void FindAvoidArea(const capi::GameState& state);
	void RemoveFromMIS_AvoidArea(capi::Tick curTick);
	std::vector<capi::Command> MoveUnitsAway(const capi::GameState& state);
	std::vector<MIS_AvoidArea *> vAvoid;
	capi::EntityId MaxAvoidID;

	//

	int CardPicker(capi::CardId opID) { return CardPicker(opID, false); };
	int CardPicker(capi::CardId opID, bool Swift);
	int GetSwiftCounterFor(Card OP, bool PerfectCounter, bool Swift);

	std::vector<capi::Command> CoolEruption(const capi::GameState& state);
	MIS_thread CoolEruptionTest;

	BattleTable CalcBattleTable(std::vector<capi::Squad> squads);
	bool CalGlobalBattleTable(const capi::GameState& state);
	MIS_thread GlobalBattleTable;
	void EchoBattleTable(BattleTable BT);

	bool Stage(const capi::GameState& state);

	
	
};

/*
auto effect_json = nlohmann::json(state).dump();
std::ofstream file("ALLat" + std::to_string(state.current_tick) + ".json");
file << effect_json << std::endl;
file.close();
*/