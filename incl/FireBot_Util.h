#define DF_Debug

#include "../incl/FireBot.h"
#include "../incl/CardBaseSMJ.h"
#include "../incl/Util.h"
#include "../incl/LOAD.h"

Card FireBot::CARD_ID_to_SMJ_CARD(capi::CardId card_id)
{
	//MISS
	//in my Deck?
	for (auto C : SMJDeck)if (C.officialCardIds == card_id % 1000000)return C;
	//in op Deck?
	for (auto C : SMJDeckOP)if (C.officialCardIds == card_id % 1000000)return C;
	//Add to OP Deck;
	SMJDeckOP.push_back(Bro->J->CardFromJson(card_id % 1000000));
	return SMJDeckOP[SMJDeckOP.size() - 1];
	//MISE;
}

std::vector<capi::Command>  FireBot::IdleToFight()
{
	bool OnWall;
	bool FoundOP;
	auto vReturn = std::vector<capi::Command>();
	capi::Entity A;
	capi::Entity B;

	for(auto S: Bro->U->FilterSquad(myId, lState.entities.squads))
		if (S.entity.job.variant_case == capi::JobCase::Idle)
		{
			OnWall = false;
			FoundOP = false;
			//Units Close By
			std::vector<capi::Squad> SOP = Bro->U->SquadsInRadius(opId, lState.entities.squads, capi::to2D(S.entity.position), Bro->L->FightRange);
			//for (auto S : STemp)
			for(unsigned int i = 0; i < SOP.size() && OnWall == false; i++)
			{
				//Do not Atack stuff on walls		
				/*
				for (unsigned int j = 0; j < SOP[i].entity.aspects.size() && OnWall == false; j++)
					if (SOP[i].entity.aspects[j].variant_case == capi::AspectCase::MountBarrier)
						OnWall = SOP[i].entity.aspects[j].variant_union.mount_barrier.state.variant_case == capi::MountStateCase::MountedSquad;
						*/	
				OnWall = onWall(SOP[i].entity);
							
				
				if (OnWall == false)
				{
					vReturn.push_back(MIS_CommandGroupAttack({ S.entity.id }, SOP[i].entity.id));
					FoundOP = true;
				}
			}

			if(FoundOP==false)
			{
				Bro->U->CloseCombi({ S.entity },
					entitiesTOentity(opId, lState.entities.power_slots, lState.entities.token_slots, lState.entities.barrier_modules), A, B); //lState.entities.squads,
				vReturn.push_back(MIS_CommandGroupGoto({ S.entity.id }, capi::to2D(B.position), capi::WalkMode_Normal));
			}
		}

	return vReturn;
}

bool FireBot::onWall(capi::Entity E)
{
	for (auto A : E.aspects)
		if (A.variant_case == capi::AspectCase::MountBarrier)
			return true;
	return false;
}

capi::EntityId FireBot::WallidOFsquad(capi::EntityId ID)
{
	for (auto S : lState.entities.squads)
		if (S.entity.id == ID)
			for (auto A : S.entity.aspects)
				if (A.variant_case == capi::AspectCase::MountBarrier)
					if (A.variant_union.mount_barrier.state.variant_case == capi::MountStateCase::MountedSquad)
						return A.variant_union.mount_barrier.state.variant_union.mounted_squad.barrier;
	return -1;
}

bool FireBot::OrbOnebOK()
{
	for (auto O : entitiesTOentity(myId, lState.entities.token_slots))if (O.id == eMainOrb.id)return true;
	return false;
}

int FireBot::GetSquadHP(capi::EntityId SquadID)
{
	int iReturn = 0;
	for (auto F : lState.entities.figures)
	{
		if (F.squad_id == SquadID)
		{
			//MISD(std::to_string(F.squad_id) + " _ " + std::to_string(F.entity.id) + " _ " + std::to_string(iReturn))
				for (auto A : F.entity.aspects)
					if (A.variant_case == capi::AspectCase::Health)
						iReturn += A.variant_union.health.current_hp;
		}
	}
	return iReturn;
}

float FireBot::GetAspect(capi::Entity E, capi::AspectCase Asp)
{
	for (auto A : E.aspects)
		if (A.variant_case == Asp && Asp == capi::AspectCase::Health)
			return A.variant_union.health.current_hp;
	return 0;
}


std::vector<capi::Command> FireBot::WellKiller()
{
	MISS;	
	auto vReturn = std::vector<capi::Command>();
	for (auto W : entitiesTOentity(opId, lState.entities.power_slots))
		for (auto A : W.aspects)
			if (A.variant_case == capi::AspectCase::Health)
				if (A.variant_union.health.current_hp <= 300)
				{
					MISD("FIRE !!!!");
					auto spell = capi::CommandCastSpellGod();
					spell.card_position = EruptionPos;
					// Spot on, maye chaneg to offset so i can hit units
					spell.target = capi::SingleTargetLocation(capi::to2D(W.position));
					vReturn.push_back(capi::Command(spell));
					return vReturn;
				}
				else break;

	MISE;
	return vReturn;
}

std::vector<capi::Command> FireBot::InstantRepairFunction()
{
	MISS;
	bool inRebpair;

	auto vReturn = std::vector<capi::Command>();
	for (auto E : entitiesTOentity(myId, lState.entities.power_slots, lState.entities.token_slots))
		for (auto A : E.aspects)
			if (A.variant_case == capi::AspectCase::Health)
				if (A.variant_union.health.current_hp != A.variant_union.health.cap_current_max)
					vReturn.push_back(MIS_CommandRepairBuilding(E.id));


	for (auto E : entitiesTOentity(myId, lState.entities.barrier_sets))
	{
		inRebpair = false;
		for (auto A : E.aspects)
			if (A.variant_case == capi::AspectCase::RepairBarrierSet)
				inRebpair = true;
		if (inRebpair == false)vReturn.push_back(MIS_CommandBarrierRepair(E.id));
	}

	MISE;
	return vReturn;
}

std::vector<capi::Command> FireBot::CoolEruption()
{
	MISS;

	auto vReturn = std::vector<capi::Command>();

	unsigned int iUnitCount;
	unsigned int iUnitCountH;
	bool flyer;

	std::vector<capi::Squad> vSquad;
	for (auto U : Bro->U->FilterSquad(opId, lState.entities.squads))
	{
		if (CARD_ID_to_SMJ_CARD(U.card_id).movementType == 1)
		{
			if (
				MoreUnitsNeeded(CalcBattleTable(Bro->U->SquadsInRadius(myId, lState.entities.squads, capi::to2D(U.entity.position), 25)),
					CalcBattleTable(Bro->U->SquadsInRadius(opId, lState.entities.squads, capi::to2D(U.entity.position), 25))))
				flyer = true;
			else flyer = false;
		}
		else flyer = false;

		vSquad = Bro->U->SquadsInRadius(opId, lState.entities.squads, capi::to2D(U.entity.position), 10);
		if (vSquad.size() >= 3)
		{

			/// Check if Units Is Skyfire and <300 HP -> Erupt

			iUnitCount = 0;
			iUnitCountH = 0;
			for (auto S : vSquad)
			{
				//Count Cout Units an wall as a eruption Target
				if (onWall(S.entity))continue;
				iUnitCount++;
				if (GetSquadHP(S.entity.id) <= 300)iUnitCountH++;
			}

			//MISD(std::to_string(iUnitCount) + " # " + std::to_string(iUnitCountH) );

			if (iUnitCountH >= 1 && iUnitCount >= 3
				|| iUnitCountH >= 2 && iUnitCount >= 2
				|| iUnitCountH >= 1 && flyer)
			{
				//MISD("FIRE !!!!");

				auto spell = capi::CommandCastSpellGod();
				spell.card_position = EruptionPos;
				spell.target = capi::SingleTargetLocation(capi::to2D(U.entity.position));
				vReturn.push_back(capi::Command(spell));
				break; //only for one unit not all 3
			}
		}
	}

	// Lava Field Code

	MISE;
	return vReturn;
}