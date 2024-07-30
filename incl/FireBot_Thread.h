#define DF_Debug

#include "../incl/FireBot.h"
#include "../incl/CardBaseSMJ.h"
#include "../incl/Util.h"
#include "../incl/LOAD.h"

std::vector<capi::Command> FireBot::CoolEruption(const capi::GameState& state)
{
	MISS;

	auto vReturn = std::vector<capi::Command>();

	unsigned int iUnitCount;
	unsigned int iUnitCountH;

	std::vector<capi::Squad> vSquad;
	for (auto U : state.entities.squads)
	{

		if (U.entity.player_entity_id != opId)continue;

		vSquad = Bro->U->SquadsInRadius(opId, state.entities.squads, capi::to2D(U.entity.position), 10);
		if (vSquad.size() >= 3)
		{

			/// Check if Units Is Skyfire and <300 HP -> Erupt

			iUnitCount = 0;
			iUnitCountH = 0;
			for (auto S : vSquad)
			{
				iUnitCount++;
				for (auto A : S.entity.aspects)
				{
					if (A.variant_case == capi::AspectCase::Health)
						if (A.variant_union.health.current_hp <= 300)iUnitCountH++;
				}
			}
			if (iUnitCountH >= 1 && iUnitCount >= 3
				|| iUnitCountH >= 2 && iUnitCount >= 2)
			{
				MISD("FIRE !!!!");

				auto spell = capi::CommandCastSpellGod();
				spell.card_position = EruptionPos;
				spell.target = capi::SingleTargetLocation(capi::to2D(U.entity.position));
				vReturn.push_back(capi::Command(spell));
				break; //only for one unit not all 3
			}
		}
	}

	MISE;
	return vReturn;
}


std::vector<capi::Command> FireBot::InstantRepairFunction(const capi::GameState& state)
{
	MISS;

	auto vReturn = std::vector<capi::Command>();
	for (auto E : entitiesTOentity(myId, state.entities.power_slots, state.entities.token_slots))
		for (auto A : E.aspects)
			if (A.variant_case == capi::AspectCase::Health)
				if (A.variant_union.health.current_hp != A.variant_union.health.cap_current_max)
					vReturn.push_back(MIS_CommandRepairBuilding(E.id));
	//MISD(vReturn.size());
	MISE;
	return vReturn;
}

bool FireBot::WellKiller(std::vector<capi::Command>& vMain, std::vector<capi::Entity> Wells)
{
	MISS;
	for (auto W : Wells)
	{
		for (auto A : W.aspects)
		{
			//if (std::get_if<capi::AspectHealth>(&A.v))
			if (A.variant_case == capi::AspectCase::Health)
			{
				if (A.variant_union.health.current_hp <= 300)
				{
					MISD("FIRE !!!!");
					auto spell = capi::CommandCastSpellGod();
					spell.card_position = EruptionPos;
					// Spot on, maye chaneg to offset so i can hit units
					spell.target = capi::SingleTargetLocation(capi::to2D(W.position));
					vMain.push_back(capi::Command(spell));
					return true;
				}
			}
		}
	}
	MISE;
	return false;
}