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

struct MIS_thread
{
	std::future<std::vector<api::Command>> f;
	std::thread t;
	bool s;

};

class FireBot : public IBotImp
{
public:
	FireBot() : IBotImp("FireBot"), myId{}, opId{}, myStart{}, iStage(0), iSkipTick(0), imyPlayerIDX(0), iopPlayerIDX(0){ };
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

	unsigned int iStage;
	unsigned int iSkipTick;

	//Instand Functions
	void WellKiller(std::vector<api::Command> vMain, std::vector<api::Entity> Wells);
	std::vector<api::Command> AvoidArea(const api::GameState& state);

	//

	int CardPicker(api::CardId opID) { return CardPicker(opID, false); };
	int CardPicker(api::CardId opID, bool Swift);
	int GetSwiftCounterFor(Card OP, bool PerfectCounter, bool Swift);

	std::vector<api::Command> CoolEruption(const api::GameState& state);
	MIS_thread CoolEruptionTest;
};