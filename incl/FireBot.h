#pragma once

#include "../incl/Broker.h"
#include "../incl/DEBUG.h"
#include "../incl/DataTypes.h"

#include "../API/API/json.hpp"
#include "../API/API/Types.hpp"
#include "../API/API/Helpers.hpp"
#include "../API/API/boost_wrapper.hpp"


class FireBot : public IBotImp
{
public:
	FireBot() : IBotImp("FireBot"), myId{}, myStart{}, iStage(0) { };
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

	api::EntityId myId;
	api::EntityId opId;
	unsigned int imyPlayerIDX;
	unsigned int iopPlayerIDX;
	api::Position2D myStart;

	unsigned int iStage;
};