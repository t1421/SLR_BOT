#pragma once
#include "../incl/Broker.h"
#include "../incl/DataTypes.h"
#include "../API/API/json.hpp"
#include "../API/API/TypesCLike.hpp"
#include "../API/API/HelpersCLike.hpp"

#include <string>
#include <vector>

struct Wall {
	capi::Position2D start;
	capi::Position2D end;
	Wall(int x1, int y1, int x2, int y2) 
	{
		start.x = x1;
		start.y = y1;
		start.x = x2;
		start.y = y2;
	};
};



namespace capi
{
	struct CommandRejectionReason;
	struct Command;
}

template<typename... Containers>
std::vector<capi::Entity> entitiesTOentity(const capi::EntityId iFilter, const Containers&... containers) 
{
	std::vector<capi::Entity> allEntities;

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
	void InitWalls(int map);

	float distance(capi::Position2D p1, capi::Position2D p2);
	float PathDistance(capi::Position2D start, capi::Position2D goal);
	float CloseCombi(std::vector<capi::Entity> EntitiesA, std::vector<capi::Entity> EntitiesB, capi::Entity& outA, capi::Entity& outB);

	bool lineIntersectsWall(const capi::Position2D& start, const capi::Position2D& goal, const Wall& wall);

	capi::Position2D WaypointTo2D(capi::Position2DWithOrientation WP);
	capi::Position2D Offseter(capi::Position2D A, capi::Position2D B, float offset) { return Offseter(A,B,offset,0); };
	capi::Position2D Offseter(capi::Position2D A, capi::Position2D B, float offset, float shift);

	std::vector<capi::Entity> pointsInRadius(std::vector<capi::Entity> toCheck, capi::Position2D Center, float Range);
	std::vector<capi::Squad> SquadsInRadius(const capi::EntityId iFilter, std::vector<capi::Squad> toCheck, capi::Position2D Center, float Range);
	std::vector<capi::Entity> EntitiesInRadius(const capi::EntityId iFilter, std::vector<capi::Entity> toCheck, capi::Position2D Center, float Range);	
	std::vector<capi::Command> DrawCircle(capi::Position2D center, float radius);
	std::vector<capi::Squad> FilterSquad(const capi::EntityId iFilter, std::vector<capi::Squad> inSquad);
	std::vector<capi::BarrierModule> FilterBarrierModule(const capi::EntityId iFilter, std::vector<capi::BarrierModule> inBarrierModule);
	
	/////////////////	
	std::string switchCommandRejectionReason(capi::CommandRejectionReason& v);
	std::string switchCommand(capi::Command& v);
	std::string switchCommandJob(capi::Job& v);
	std::string switchAbilityEffectSpecific(capi::AbilityEffectSpecific& v);
	std::string switchAreaShape(capi::AreaShape& v);
	std::string switchAspect(capi::Aspect& v);
	std::string switchMountStatet(capi::MountState& v);
	/////////////////

	static broker* Bro;
	void teachU() { Bro->U = this; }
	static void learnBro(broker *_Bro) { Bro = _Bro; }	

protected:

private:
	std::vector<Wall> walls;
	
};
