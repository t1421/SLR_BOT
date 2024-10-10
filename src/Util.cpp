#include "../incl/Util.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

broker *(Util::Bro) = NULL;


std::string Util::switchCommandRejectionReason(capi::CommandRejectionReason& v)
{	
	capi::CommandRejectionReasonCase variant_case = v.variant_case;
	switch (variant_case)
	{
	case capi::CommandRejectionReasonCase::Invalid: return "Invalid default variant!";
	case capi::CommandRejectionReasonCase::CardRejected: return "CardRejected";
	case capi::CommandRejectionReasonCase::NotEnoughPower: return "NotEnoughPower";
	case capi::CommandRejectionReasonCase::SpellDoesNotExist: return "SpellDoesNotExist";
	case capi::CommandRejectionReasonCase::EntityDoesNotExist: return "EntityDoesNotExist";
	case capi::CommandRejectionReasonCase::InvalidEntityType: return "InvalidEntityType";
	case capi::CommandRejectionReasonCase::CanNotCast: return "CanNotCast";
	case capi::CommandRejectionReasonCase::EntityNotOwned: return "EntityNotOwned";
	case capi::CommandRejectionReasonCase::EntityOwnedBySomeoneElse: return "EntityOwnedBySomeoneElse";
	case capi::CommandRejectionReasonCase::NoModeChange: return "NoModeChange";
	case capi::CommandRejectionReasonCase::EntityAlreadyInThisMode: return "EntityAlreadyInThisMode";
	case capi::CommandRejectionReasonCase::ModeNotExist: return "ModeNotExist";
	case capi::CommandRejectionReasonCase::InvalidCardIndex: return "InvalidCardIndex";
	case capi::CommandRejectionReasonCase::InvalidCard: return "InvalidCard";
	default: return "invalid case";
	}
}

std::string Util::switchCommand(capi::Command& v)
{
	capi::CommandCase variant_case = v.variant_case;
	switch (variant_case)
	{
	case capi::CommandCase::Invalid: return "Invalid default variant!";
	case capi::CommandCase::BuildHouse: return "BuildHouse";
	case capi::CommandCase::CastSpellGod: return "CastSpellGod";
	case capi::CommandCase::CastSpellGodMulti: return "CastSpellGodMulti";
	case capi::CommandCase::ProduceSquad: return "ProduceSquad";
	case capi::CommandCase::ProduceSquadOnBarrier: return "ProduceSquadOnBarrier";
	case capi::CommandCase::CastSpellEntity: return "CastSpellEntity";
	case capi::CommandCase::BarrierGateToggle: return "BarrierGateToggle";
	case capi::CommandCase::BarrierBuild: return "BarrierBuild";
	case capi::CommandCase::BarrierRepair: return "BarrierRepair";
	case capi::CommandCase::BarrierCancelRepair: return "BarrierCancelRepair";
	case capi::CommandCase::RepairBuilding: return "RepairBuilding";
	case capi::CommandCase::CancelRepairBuilding: return "CancelRepairBuilding";
	case capi::CommandCase::GroupAttack: return "GroupAttack";
	case capi::CommandCase::GroupEnterWall: return "GroupEnterWall";
	case capi::CommandCase::GroupExitWall: return "GroupExitWall";
	case capi::CommandCase::GroupGoto: return "GroupGoto";
	case capi::CommandCase::GroupHoldPosition: return "GroupHoldPosition";
	case capi::CommandCase::GroupStopJob: return "GroupStopJob";
	case capi::CommandCase::ModeChange: return "ModeChange";
	case capi::CommandCase::PowerSlotBuild: return "PowerSlotBuild";
	case capi::CommandCase::TokenSlotBuild: return "TokenSlotBuild";
	case capi::CommandCase::Ping: return "Ping";
	case capi::CommandCase::Surrender: return "Surrender";
	case capi::CommandCase::WhisperToMaster: return "WhisperToMaster";
	default: return "invalid case";
	}
}

std::string Util::switchCommandJob(capi::Job& v)
{
	capi::JobCase variant_case = v.variant_case;
	switch (variant_case) {
	case capi::JobCase::Invalid: return "Invalid default variant!";
	case capi::JobCase::NoJob: return "NoJob";
	case capi::JobCase::Idle: return "Idle";
	case capi::JobCase::Goto: return "Goto";
	case capi::JobCase::AttackMelee: return "AttackMelee";
	case capi::JobCase::CastSpell: return "CastSpell";
	case capi::JobCase::Die: return "Die";
	case capi::JobCase::Talk: return "Talk";
	case capi::JobCase::ScriptTalk: return "ScriptTalk";
	case capi::JobCase::Freeze: return "Freeze";
	case capi::JobCase::Spawn: return "Spawn";
	case capi::JobCase::Cheer: return "Cheer";
	case capi::JobCase::AttackSquad: return "AttackSquad";
	case capi::JobCase::CastSpellSquad: return "CastSpellSquad";
	case capi::JobCase::PushBack: return "PushBack";
	case capi::JobCase::Stampede: return "Stampede";
	case capi::JobCase::BarrierCrush: return "BarrierCrush";
	case capi::JobCase::BarrierGateToggle: return "BarrierGateToggle";
	case capi::JobCase::FlameThrower: return "FlameThrower";
	case capi::JobCase::Construct: return "Construct";
	case capi::JobCase::Crush: return "Crush";
	case capi::JobCase::MountBarrierSquad: return "MountBarrierSquad";
	case capi::JobCase::MountBarrier: return "MountBarrier";
	case capi::JobCase::ModeChangeSquad: return "ModeChangeSquad";
	case capi::JobCase::ModeChange: return "ModeChange";
	case capi::JobCase::SacrificeSquad: return "SacrificeSquad";
	case capi::JobCase::UsePortalSquad: return "UsePortalSquad";
	case capi::JobCase::Channel: return "Channel";
	case capi::JobCase::SpawnSquad: return "SpawnSquad";
	case capi::JobCase::LootTargetSquad: return "LootTargetSquad";
	case capi::JobCase::Morph: return "Morph";
	case capi::JobCase::Unknown: return "Unknown";
	default: return "Invalid variant";
	}
}

std::string Util::switchAbilityEffectSpecific(capi::AbilityEffectSpecific& v)
{
	capi::AbilityEffectSpecificCase variant_case = v.variant_case;
	switch (variant_case) {
	case capi::AbilityEffectSpecificCase::Invalid: return "Invalid default variant!";
	case capi::AbilityEffectSpecificCase::DamageArea: return "DamageArea";
	case capi::AbilityEffectSpecificCase::DamageOverTime: return "DamageOverTime";
	case capi::AbilityEffectSpecificCase::LinkedFire: return "LinkedFire";
	case capi::AbilityEffectSpecificCase::SpellOnEntityNearby: return "SpellOnEntityNearby";
	case capi::AbilityEffectSpecificCase::TimedSpell: return "TimedSpell";
	case capi::AbilityEffectSpecificCase::Collector: return "Collector";
	case capi::AbilityEffectSpecificCase::Aura: return "Aura";
	case capi::AbilityEffectSpecificCase::MovingIntervalCast: return "MovingIntervalCast";
	case capi::AbilityEffectSpecificCase::Other: return "Other";
	default: return "Invalid variant";
	}

}

std::string Util::switchAreaShape(capi::AreaShape& v)
{
	capi::AreaShapeCase variant_case = v.variant_case;
	switch (variant_case) {
	case capi::AreaShapeCase::Invalid: return "Invalid default variant!";
	case capi::AreaShapeCase::Circle: return "Circle";
	case capi::AreaShapeCase::Cone: return "Cone";
	case capi::AreaShapeCase::ConeCut: return "ConeCut";
	case capi::AreaShapeCase::WideLine: return "WideLine";
	default: return "invalid case";
	}
}


std::string Util::switchAspect(capi::Aspect& v)
{
	capi::AspectCase variant_case = v.variant_case;
	switch (variant_case) {
	case capi::AspectCase::Invalid: return "Invalid default variant!";
	case capi::AspectCase::PowerProduction: return "PowerProduction";
	case capi::AspectCase::Health: return "Health";
	case capi::AspectCase::Combat: return "Combat";
	case capi::AspectCase::ModeChange: return "ModeChange";
	case capi::AspectCase::Ammunition: return "Ammunition";
	case capi::AspectCase::SuperWeaponShadow: return "SuperWeaponShadow";
	case capi::AspectCase::WormMovement: return "WormMovement";
	case capi::AspectCase::NPCTag: return "NPCTag";
	case capi::AspectCase::PlayerKit: return "PlayerKit";
	case capi::AspectCase::Loot: return "Loot";
	case capi::AspectCase::Immunity: return "Immunity";
	case capi::AspectCase::Turret: return "Turret";
	case capi::AspectCase::Tunnel: return "Tunnel";
	case capi::AspectCase::MountBarrier: return "MountBarrier";
	case capi::AspectCase::SpellMemory: return "SpellMemory";
	case capi::AspectCase::Portal: return "Portal";
	case capi::AspectCase::Hate: return "Hate";
	case capi::AspectCase::BarrierGate: return "BarrierGate";
	case capi::AspectCase::Attackable: return "Attackable";
	case capi::AspectCase::SquadRefill: return "SquadRefill";
	case capi::AspectCase::PortalExit: return "PortalExit";
	case capi::AspectCase::ConstructionData: return "ConstructionData";
	case capi::AspectCase::SuperWeaponShadowBomb: return "SuperWeaponShadowBomb";
	case capi::AspectCase::RepairBarrierSet: return "RepairBarrierSet";
	case capi::AspectCase::ConstructionRepair: return "ConstructionRepair";
	case capi::AspectCase::Follower: return "Follower";
	case capi::AspectCase::CollisionBase: return "CollisionBase";
	case capi::AspectCase::EditorUniqueID: return "EditorUniqueID";
	case capi::AspectCase::Roam: return "Roam";
	default: return "Invalid variant";
	}
}
std::string Util::switchMountStatet(capi::MountState& v)
{
	capi::MountStateCase variant_case = v.variant_case;
	switch (variant_case)	{
	case capi::MountStateCase::Invalid: return "Invalid";
	case capi::MountStateCase::Unmounted: return "unmounted";
	case capi::MountStateCase::MountingSquad: return "mounting_squad";
	case capi::MountStateCase::MountingFigure: return "mounting_figure";
	case capi::MountStateCase::MountedSquad: return "mounted_squad";
	case capi::MountStateCase::MountedFigure: return "mounted_figure";
	case capi::MountStateCase::Unknown: return "unknown";
	default: return "Invalid variant";
	}
}


std::vector<capi::Entity> Util::pointsInRadius(std::vector<capi::Entity> toCheck, capi::Position2D Center, float Range)
{
	std::vector<capi::Entity> vReturn;
	for (auto e : toCheck)if (distance(capi::to2D(e.position), Center) <= Range)vReturn.push_back(e);
	return vReturn;
}

std::vector<capi::Squad> Util::SquadsInRadius(const capi::EntityId iFilter, std::vector<capi::Squad> toCheck, capi::Position2D Center, float Range)
{
	std::vector<capi::Squad> vReturn;
	for (auto s : toCheck)
		if(s.entity.player_entity_id == iFilter)
			if (distance(capi::to2D(s.entity.position), Center) <= Range)
				vReturn.push_back(s);
	return vReturn;
}

std::vector<capi::Entity> Util::EntitiesInRadius(const capi::EntityId iFilter, std::vector<capi::Entity> toCheck, capi::Position2D Center, float Range)
{
	std::vector<capi::Entity> vReturn;
	for (auto e : toCheck)
		if (e.player_entity_id == iFilter)
			if (distance(capi::to2D(e.position), Center) <= Range)
				vReturn.push_back(e);
	return vReturn;
}

float Util::distance(capi::Position2D p1, capi::Position2D p2)
{
	return float(sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2)));
}

capi::Position2D Util::Offseter(capi::Position2D A, capi::Position2D B, float offset, float shift) 
{
	float totalDistance = distance(A, B);
	float ratio = offset / totalDistance;

	float offsetX = (B.x - A.x) * ratio;
	float offsetY = (B.y - A.y) * ratio;

	// Seitlichen Offset berechnen
	float normalX = B.y - A.y;
	float normalY = A.x - B.x;
	float normalLength = std::sqrt(normalX * normalX + normalY * normalY);
	normalX /= normalLength;
	normalY /= normalLength;

	offsetX += normalX * shift;
	offsetY += normalY * shift;

	// Berechnung der Position des ermittelten Punktes
	capi::Position2D offsetPoint;
	offsetPoint.x = A.x + offsetX;
	offsetPoint.y = A.y + offsetY;

	return offsetPoint;
}

float Util::CloseCombi(std::vector<capi::Entity> EntitiesA, std::vector<capi::Entity> EntitiesB, capi::Entity& outA, capi::Entity& outB)
{
	float topDistance = 99999;
	float DistanceTemp = 0;
	//for (unsigned int iA = 0; iA < EntitiesA.size(); iA++)
	for (auto A: EntitiesA)
		for (auto B : EntitiesB)
		{
			DistanceTemp = distance(capi::to2D(A.position), capi::to2D(B.position));
			if (DistanceTemp < topDistance)
			{
				topDistance = DistanceTemp;
				outA = A;
				outB = B;
			}
		}
	return topDistance;
}

std::vector<capi::Command> Util::DrawCircle(capi::Position2D center, float radius)
{
	auto vReturn = std::vector<capi::Command>();
	auto ping = capi::CommandPing();
	ping.ping = capi::Ping::Ping_Attention;

	for (int i = 0; i < 8; ++i) {
		float angle = 2 * M_PI * i / 8; // Winkel berechnen
		float offsetX = radius * cos(angle); // x-Offset berechnen
		float offsetY = radius * sin(angle); // y-Offset berechnen

		ping.xy = { center.x + offsetX, center.y + offsetY };
		vReturn.push_back(capi::Command(ping));
	}

	return vReturn;
}


std::vector<capi::Squad> Util::FilterSquad(const capi::EntityId iFilter, std::vector<capi::Squad> inSquad)
{
	std::vector<capi::Squad> allSquad;

	for (const auto& s : inSquad)
	{
		if (iFilter == 0 && s.entity.player_entity_id.has_value())continue;
		if (iFilter != 0 && !s.entity.player_entity_id.has_value())continue;
		if (iFilter != 0 && s.entity.player_entity_id != iFilter)continue;
		allSquad.push_back(s);
	}

	return allSquad;
}

