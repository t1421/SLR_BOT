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
	case 0: return "Invalid switchCommandRejectionReason"; break;
	case 1: return "Other"; break;
	case 2: return "NotEnoughPower"; break;
	case 3: return "SpellDoesNotExist"; break;
	case 4: return "EntityDoesNotExist"; break;
	case 5: return "InvalidEntityType"; break;
	case 6: return "CanNotCast"; break;
	case 7: return "EntityNotOwned"; break;
	case 8: return "EntityOwnedBySomeoneElse"; break;
	case 9: return "NoModeChange"; break;
	case 10: return "EntityAlreadyInThisMode"; break;
	case 11: return "ModeNotExist"; break;
	case 12: return "InvalidCardIndex"; break;
	case 13: return "InvalidCard"; break;
	default: return "invalid case";
	}
}

std::string Util::switchCommand(api::Command& v)
{
	switch (v.v.index()) 
	{
	case 0: return "Invalid switchCommand"; break;
	case 1: return "BuildHouse"; break;
	case 2: return "CastSpellGod"; break;
	case 3: return "CastSpellGodMulti"; break;
	case 4: return "ProduceSquad"; break;
	case 5: return "ProduceSquadOnBarrier"; break;
	case 6: return "CastSpellEntity"; break;
	case 7: return "BarrierGateToggle"; break;
	case 8: return "BarrierBuild"; break;
	case 9: return "BarrierRepair"; break;
	case 10: return "BarrierCancelRepair"; break;
	case 11: return "RepairBuilding"; break;
	case 12: return "CancelRepairBuilding"; break;
	case 13: return "GroupAttack"; break;
	case 14: return "GroupEnterWall"; break;
	case 15: return "GroupExitWall"; break;
	case 16: return "GroupGoto"; break;
	case 17: return "GroupHoldPosition"; break;
	case 18: return "GroupStopJob"; break;
	case 19: return "ModeChange"; break;
	case 20: return "PowerSlotBuild"; break;
	case 21: return "TokenSlotBuild"; break;
	case 22: return "Ping"; break;
	case 23: return "Surrender"; break;
	case 24: return "WhisperToMaster"; break;
	default: return "invalid case";
	}
}

std::string Util::switchCommandJob(api::Job& v)
{
	switch (v.v.index()) {
	case 0: return "Invalid switchCommandJob"; break;
	case 2: return "Idle"; break;
	case 3: return "Goto"; break;
	case 4: return "AttackMelee"; break;
	case 5: return "CastSpell"; break;
	case 6: return "Die"; break;
	case 7: return "Talk"; break;
	case 8: return "ScriptTalk"; break;
	case 9: return "Freeze"; break;
	case 10: return "Spawn"; break;
	case 11: return "Cheer"; break;
	case 12: return "AttackSquad"; break;
	case 13: return "CastSpellSquad"; break;
	case 14: return "PushBack"; break;
	case 15: return "Stampede"; break;
	case 16: return "BarrierCrush"; break;
	case 17: return "BarrierGateToggle"; break;
	case 18: return "FlameThrower"; break;
	case 19: return "Construct"; break;
	case 20: return "Crush"; break;
	case 21: return "MountBarrierSquad"; break;
	case 22: return "MountBarrier"; break;
	case 23: return "ModeChangeSquad"; break;
	case 24: return "ModeChange"; break;
	case 25: return "SacrificeSquad"; break;
	case 26: return "UsePortalSquad"; break;
	case 27: return "Channel"; break;
	case 28: return "SpawnSquad"; break;
	case 29: return "LootTargetSquad"; break;
	case 30: return "Morph"; break;
	case 31: return "Unknown"; break;
	default: return "invalid case";
	}
}

std::string Util::switchAbilityEffectSpecific(api::AbilityEffectSpecific& v)
{
	switch (v.v.index()) {
	case 0: return "Invalid switchAbilityEffectSpecific"; break;
	case 1: return "DamageArea"; break;
	case 2: return "DamageOverTime"; break;
	case 3: return "LinkedFire"; break;	
	case 4: return "SpellOnEntityNearby"; break;
	case 5: return "TimedSpell"; break;
	case 6: return "Collector"; break;
	case 7: return "Aura"; break;
	case 8: return "MovingIntervalCast"; break;
	case 9: return "Other"; break;
	default: return "invalid case";
	}
}

std::string Util::switchAreaShape(api::AreaShape& v)
{
	switch (v.v.index()) {
	case 0: return "Invalid switchAreaShape"; break;
	case 1: return "Circle"; break;
	case 2: return "Cone"; break;
	case 3: return "ConeCut"; break;
	case 4: return "WideLine"; break;
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
	return float(sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2)));
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

