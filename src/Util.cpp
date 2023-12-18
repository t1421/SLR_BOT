#include "../incl/Util.h"




broker *(Util::Bro) = NULL;

/*
std::string Util::switchAPIEntitySpecific(api::APIEntitySpecific& v)
{
	switch (v.v.index()) 
	{
	case 0: return "Projectile"; break;
	case 1: return "PowerSlot"; break;
	case 2: return "TokenSlot"; break;
	case 3: return "AbilityWorldObject"; break;
	case 4: return "Squad"; break;
	case 5: return "Figure"; break;
	case 6: return "Building"; break;
	case 7: return "BarrierSet"; break;
	case 8: return "BarrierModule"; break;
	default: return "invalid case";
	}
}
*/
std::string Util::switchCommandRejectionReason(api::CommandRejectionReason& v)
{
	switch (v.v.index()) 
	{
	case 0: return "Other"; break;
	case 1: return "NotEnoughPower"; break;
	case 2: return "SpellDoesNotExist"; break;
	case 3: return "EntityDoesNotExist"; break;
	case 4: return "InvalidEntityType"; break;
	case 5: return "CanNotCast"; break;
	case 6: return "EntityNotOwned"; break;
	case 7: return "EntityOwnedBySomeoneElse"; break;
	case 8: return "NoModeChange"; break;
	case 9: return "EntityAlreadyInThisMode"; break;
	case 10: return "ModeNotExist"; break;
	case 11: return "InvalidCardIndex"; break;
	case 12: return "InvalidCard"; break;
	default: return "invalid case";
	}
}

std::string Util::switchCommand(api::Command& v)
{
	switch (v.v.index()) 
	{
	case 0: return "BuildHouse"; break;
	case 1: return "CastSpellGod"; break;
	case 2: return "CastSpellGodMulti"; break;
	case 3: return "ProduceSquad"; break;
	case 4: return "ProduceSquadOnBarrier"; break;
	case 5: return "CastSpellEntity"; break;
	case 6: return "BarrierGateToggle"; break;
	case 7: return "BarrierBuild"; break;
	case 8: return "BarrierRepair"; break;
	case 9: return "BarrierCancelRepair"; break;
	case 10: return "RepairBuilding"; break;
	case 11: return "CancelRepairBuilding"; break;
	case 12: return "GroupAttack"; break;
	case 13: return "GroupEnterWall"; break;
	case 14: return "GroupExitWall"; break;
	case 15: return "GroupGoto"; break;
	case 16: return "GroupHoldPosition"; break;
	case 17: return "GroupStopJob"; break;
	case 18: return "ModeChange"; break;
	case 19: return "PowerSlotBuild"; break;
	case 20: return "TokenSlotBuild"; break;
	case 21: return "Ping"; break;
	case 22: return "Surrender"; break;
	case 23: return "WhisperToMaster"; break;
	default: return "invalid case";
	}
}

std::vector<api::Entity> Util::pointsInRadius(std::vector<api::Entity> toCheck, api::Position2D Center, float Range)
{
	std::vector<api::Entity> vReturn;
	for (auto e : toCheck)if (distance(api::to2D(e.position), Center) <= Range)vReturn.push_back(e);
	return vReturn;
}

float Util::distance(api::Position2D p1, api::Position2D p2)
{
	return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

api::Position2D Util::A_B_Offsetter(api::Position2D A, api::Position2D B, float Range)
{
	float ratio = Range / distance(A, B);
	api::Position2D Preturn;
	if (ratio > 1)
	{
		Preturn.x = B.x;
		Preturn.y = B.y;
		return Preturn;
	}

	Preturn.x = A.x + ratio * (B.x - A.x);
	Preturn.y = A.y + ratio * (B.y - A.y);
	return Preturn;
}

float Util::CloseCombi(std::vector<api::Entity> EntitiesA, std::vector<api::Entity> EntitiesB, api::Entity& outA, api::Entity& outB)
{
	float topDistance = 99999;
	float DistanceTemp = 0;
	//for (unsigned int iA = 0; iA < EntitiesA.size(); iA++)
	for (auto A: EntitiesA)
		for (auto B : EntitiesB)
		{
			DistanceTemp = distance(api::to2D(A.position), api::to2D(B.position));
			if (DistanceTemp < topDistance)
			{
				topDistance = DistanceTemp;
				outA = A;
				outB = B;
			}
		}
	return topDistance;
}

