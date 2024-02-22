#pragma once
#include "../incl/Broker.h"

#include "../API/API/json.hpp"
#include "../API/API/Types.hpp"
#include "../API/API/Helpers.hpp"

#include <string>
#include <vector>

namespace api
{
	struct CommandRejectionReason;
	struct Command;
}

template<typename... Containers>
std::vector<api::Entity> entitiesTOentity(const api::EntityId iFilter, const Containers&... containers) {
	std::vector<api::Entity> allEntities;

	auto processEntities = [&allEntities, iFilter](const auto& entities) {
		for (const auto& e : entities) 
		{
			if (iFilter == 0 && e.entity.player_entity_id.has_value())continue;
			if (iFilter != 0 && ! e.entity.player_entity_id.has_value())continue;
			if (iFilter != 0 && e.entity.player_entity_id != iFilter)continue;
			allEntities.push_back(e.entity);			
		}
		};

	(processEntities(containers), ...);

	return allEntities;
}

class Util
{
public:
	float distance(api::Position2D p1, api::Position2D p2);
	api::Position2D A_B_Offsetter(api::Position2D A, api::Position2D B, float Range);
	std::vector<api::Entity> pointsInRadius(std::vector<api::Entity> toCheck, api::Position2D Center, float Range);
	float CloseCombi(std::vector<api::Entity> EntitiesA, std::vector<api::Entity> EntitiesB, api::Entity& outA, api::Entity& outB);

	

	/////////////////	
	std::string switchCommandRejectionReason(api::CommandRejectionReason& v);
	std::string switchCommand(api::Command& v);
	std::string switchCommandJob(api::Job& v);
	/////////////////

	static broker* Bro;
	void teachU() { Bro->U = this; }
	static void learnBro(broker *_Bro) { Bro = _Bro; }	

protected:

private:

};
