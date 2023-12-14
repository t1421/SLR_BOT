//#define DF_Debug

#include "../incl/FireBot.h"

broker* (FireBot::Bro) = NULL;

float distance(api::Position p1, api::Position p2)
{
	return sqrt(pow(p2.x - p1.x, 2) + pow(p2.z - p1.z, 2));
}

api::Position2D A_B_Offsetter(api::Position A, api::Position B, float Range)
{
	float ratio = Range / distance(A, B);
	api::Position2D Preturn;
	if (ratio > 1)
	{
		Preturn.x = B.x;
		Preturn.y = B.z;
		return Preturn;
	}

	Preturn.x = A.x + ratio * (B.x - A.x);
	Preturn.y = A.z + ratio * (B.z - A.z);
	return Preturn;
}

float CloseCombi(std::vector<api::APIEntity> EntitiesA, std::vector<api::APIEntity> EntitiesB, unsigned int& outA, unsigned int& outB)
{
	outA = 0;
	outB = 0;
	float topDistance = 99999;
	float DistanceTemp = 0;
	for (unsigned int iA = 0; iA < EntitiesA.size(); iA++)
		for (unsigned int iB = 0; iB < EntitiesB.size(); iB++)
		{
			DistanceTemp = distance(EntitiesA[iA].position, EntitiesB[iB].position);
			if (DistanceTemp < topDistance)
			{
				topDistance = DistanceTemp;
				outA = iA;
				outB = iB;
			}
		}
	return topDistance;
}

std::vector<api::APIEntity> pointsInRadius(std::vector<api::APIEntity> toCheck, api::Position Center, float Range) {
	std::vector<api::APIEntity> vReturn;
	for (auto e : toCheck)if (distance(e.position,Center) <= Range)vReturn.push_back(e);
	return vReturn;
}

////////////////////////////////



void FireBot::PrepareForBattle(const api::MapInfo& mapInfo, const api::DeckAPI& deck){
	MISS;
	std::cout << "Prepare for: " << mapInfo.map << std::endl;
	oponents.clear();
	MISE;
}


std::vector<api::DeckAPI> FireBot::DecksForMap(const api::MapInfo& mapInfo) {
	MISS;	
	
	MISD("MAP: " + mapInfo.map);
	//MISD("COM: " + mapInfo.community_map_details->name);
	unsigned int i = 0;
	auto v = std::vector<api::DeckAPI>();
	auto deck = api::DeckAPI();
	deck.name = "FireDeck";
	deck.cover_card_index = 19;
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::Scavenger, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::Sunstriders, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::Thugs, api::Upgrade_U3);	
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::Wrecker, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::Sunderer, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::FireswornAFire, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::Eruption, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::ScorchedEarthAFire, api::Upgrade_U3);

	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::Enforcer, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::FiredancerPromo, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::ScytheFiends, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::SkyfireDrake, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::GladiatrixANature, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::LavaField, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::Wildfire, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::Ravage, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::RallyingBanner, api::Upgrade_U3);
	
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::JuggernautPromo, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::GiantSlayer, api::Upgrade_U3);
	deck.cards[i++] = api::CardIdWithUpgrade(card_templates::Spitfire, api::Upgrade_U3);

	v.push_back(deck);

	MISE;
	return v;
}

void FireBot::MatchStart(const api::APIGameStartState& state) {
	MISS;
	
	for (auto p : state.players)
	{
		if (p.entity.id != state.your_player_id) opId = p.entity.id;
	}
	myId = state.your_player_id;
	MISD("MY ID: " + std::to_string(myId));
	MISD("OP ID: " + std::to_string(opId));
		
	MISE;
}

void FireBot::UpdateStuff(const api::APIGameState& state)
{
	MISS;

	MyBuildings.clear();
	MyUnits.clear();
	OpUnits.clear();
	FreeWells.clear();
	FreeOrbs.clear();
	OpWells.clear();

	for (auto& e : state.entities)
	{
		if ((std::get_if<api::APIEntitySpecificPowerSlot>(&e.specific.v) ||
			std::get_if<api::APIEntitySpecificTokenSlot>(&e.specific.v))
			&& e.player_entity_id == myId)
			MyBuildings.push_back(e);

		if (std::get_if<api::APIEntitySpecificSquad>(&e.specific.v)
			&& e.player_entity_id == myId)
			MyUnits.push_back(e);

		if (std::get_if<api::APIEntitySpecificSquad>(&e.specific.v)
			&& e.player_entity_id == opId)
			OpUnits.push_back(e);

		if (std::get_if<api::APIEntitySpecificPowerSlot>(&e.specific.v) 
			&& e.player_entity_id != myId
			&& e.player_entity_id != opId)
			FreeWells.push_back(e);

		if (std::get_if<api::APIEntitySpecificTokenSlot>(&e.specific.v)
			&& e.player_entity_id != myId
			&& e.player_entity_id != opId)
			FreeOrbs.push_back(e);

		if (std::get_if<api::APIEntitySpecificPowerSlot>(&e.specific.v)
			&& e.player_entity_id == opId)
			OpWells.push_back(e);
	}

	MISE;

}

std::vector<api::APICommand> FireBot::Tick(const api::APIGameState& state) {
	//MISS;
	MISD(std::to_string(iStage) +  "#" + Bro->sTime(state.current_tick) + "#" + std::to_string(state.current_tick));
	//std::cout << "tick " << state.current_tick << " entities count: " << state.entities.size() << std::endl;
	auto v = std::vector<api::APICommand>();
	
	UpdateStuff(state);

	for (auto r : state.rejected_commands)
	{
		MISD("rejected Player: " + std::to_string(r.player));		
		MISD("reason         : " + Bro->B->switchCommandRejectionReason(r.reason));
		MISD("command        : " + Bro->B->switchAPICommand(r.command));
	}

	if (once)
	{
		if (state.players.size() - 1 < iPlayerCount)
			for (iPlayerCount = 0; iPlayerCount < state.players.size(); iPlayerCount++)
				if (state.players[iPlayerCount].id == myId)
					imyPlayerIDX = iPlayerCount;
		
		MISD("MyBuildings");
		for (auto e : MyBuildings)
		{
			MISD(std::to_string(e.id) + "#" +
				//std::to_string(e.player_entity_id.value()) + "#" +
				std::to_string(e.position.x) + "_" +
				std::to_string(e.position.y) + "_" +
				std::to_string(e.position.z) + "#" +
				Bro->B->switchAPIEntitySpecific(e.specific));
		}

		MISD("FreeWells");
		for (auto e : FreeWells)
		{
			MISD(std::to_string(e.id) + "#" +
				//std::to_string(e.player_entity_id.value()) + "#" +
				std::to_string(e.position.x) + "_" +
				std::to_string(e.position.y) + "_" +
				std::to_string(e.position.z) + "#" +
				Bro->B->switchAPIEntitySpecific(e.specific));
		}
		MISD("FreeOrbs");
		for (auto e : FreeOrbs)
		{
			MISD(std::to_string(e.id) + "#" +
				//std::to_string(e.player_entity_id.value()) + "#" +
				std::to_string(e.position.x) + "_" +
				std::to_string(e.position.y) + "_" +
				std::to_string(e.position.z) + "#" +
				Bro->B->switchAPIEntitySpecific(e.specific));
		}

		once = false;
	}

	

	//WELL KILLER
	if (true)
	{
		for (auto W : OpWells)
		{
			for (auto A : W.aspects)
			{
				if (std::get_if<api::AspectHealth>(&A.v))
				{
					if (std::get<api::AspectHealth>(A.v).current_hp <= 300)
					{
						MISD("FIRE !!!!");
						auto spell = api::APICommandCastSpellGod();
						spell.card_position = 6;
						spell.target.v = api::SingleTargetLocation(api::to2D(W.position));
						auto cmd = api::APICommand();
						cmd.v = spell;
						v.push_back(cmd);
					}
				}							
			}
		}
	}

	//Cool eruption
	if (true)
	{
		std::vector<api::APIEntity> vTemp;
		for (auto U : OpUnits)
		{
			vTemp = pointsInRadius(OpUnits, U.position, 10);
			if (vTemp.size() >=3)
			{
				MISD("FIRE !!!!");
				auto spell = api::APICommandCastSpellGod();
				spell.card_position = 6;
				spell.target.v = api::SingleTargetLocation(api::to2D(U.position));
				auto cmd = api::APICommand();
				cmd.v = spell;
				v.push_back(cmd);
			}			
		}
	}

	if (MyBuildings.size() < 5 && state.current_tick % 5)
	{
		unsigned int iHelperA = 0;
		unsigned int iHelperB = 0;
		float fDistanc = 0;
		
		if(MyUnits.size() == 0)fDistanc = CloseCombi(MyBuildings, FreeWells, iHelperA, iHelperB);
		else fDistanc = CloseCombi(MyUnits, FreeWells, iHelperA, iHelperB);
		
		//habe keine unit also hol eine
		if (MyUnits.size() == 0)
		{
			if (state.players[imyPlayerIDX].power > 75)
			{
				auto spawn = api::APICommandProduceSquad();
				spawn.card_position = 0; // Code für fast unit
				//spawn.xy = api::to2D(MyBuildings[iHelperA].position);				
				spawn.xy = A_B_Offsetter(MyBuildings[iHelperA].position, FreeWells[iHelperB].position, CastRange);
				MISD("My Building: " + std::to_string(MyBuildings[iHelperA].position.x) + "#" + std::to_string(MyBuildings[iHelperA].position.z));
				MISD("My Traget  : " + std::to_string(FreeWells[iHelperB].position.x) + "#" + std::to_string(FreeWells[iHelperB].position.z));
				MISD("SPawn at   : " + std::to_string(spawn.xy.x) + "#" + std::to_string(spawn.xy.y));
				MISD("Distanc    : " + std::to_string(CastRange));
				auto cmd = api::APICommand();
				cmd.v = spawn;
				v.push_back(cmd);
			}
		}
		else //Move Unit
		{
			auto move = api::APICommandGroupGoto();
			move.squads = { MyUnits[iHelperA].id};
			move.positions = { api::to2D(FreeWells[iHelperB].position) };
			move.walk_mode = api::WalkMode_Normal;
			auto cmd = api::APICommand();
			cmd.v = move;
			v.push_back(cmd);
		}

		if (fDistanc < CastRange && state.players[imyPlayerIDX].power >= 100)
		{
			auto build = api::APICommandPowerSlotBuild();
			build.slot_id = FreeWells[iHelperB].id;
			auto cmd = api::APICommand();
			cmd.v = build;
			v.push_back(cmd);
			iStage++;
		}
	}
	
	//MISE;
	return v;
}



void run_FireBot(broker* Bro, unsigned short port)
{
	MISS;
	FireBot::learnBro(Bro);
	auto bot = FireBot();		
	run(bot, port);
	MISE;
}