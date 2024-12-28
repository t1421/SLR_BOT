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
	MISS;

	bool OnWall;
	bool FoundOP;
	auto vReturn = std::vector<capi::Command>();
	capi::Entity A;
	capi::Entity B;
	capi::EntityId WallModul;

	//Remove Old Entries
	for (std::vector<MIS_Ideler*>::iterator it = vIdler.begin(); it != vIdler.end();)
		if ((*it)->endTick <= lState.current_tick)it = vIdler.erase(it);
		else ++it;

	for(auto S: Bro->U->FilterSquad(myId, lState.entities.squads))
		if (squadIsIdle(S.entity.id))
		{
			//MISD(S.entity.id);
			OnWall = false;
			FoundOP = false;

			//Units Close By
			std::vector<capi::Squad> SOP = Bro->U->SquadsInRadius(opId, lState.entities.squads, capi::to2D(S.entity.position), Bro->L->FightRange);
			for(unsigned int i = 0; i < SOP.size() && OnWall == false; i++)
			{

				OnWall = onWall(SOP[i].entity);
							
				if (OnWall == false)
				{
					//MISD("Ground");
					WallModul = behindWall(S.entity, SOP[i].entity);
					//Not behind a wall, or Wall as has a gap -> Atack Unit
					if (   WallModul == 0
						|| WallModul != 0 && GapInWall(SetOfWallmodul(WallModul)))
					{
						//MISD("On  Wall" + std::to_string(S.entity.id) + " ATTACK " + std::to_string(SOP[i].entity.id));
						vReturn.push_back(MIS_CommandGroupAttack({ S.entity.id }, SOP[i].entity.id));
						FoundOP = true;
					}
					//Als ATack Wall
					else
					{
						//MISD("The Wall" + std::to_string(S.entity.id) + " ATTACK " + std::to_string(WallModul));
						vReturn.push_back(MIS_CommandGroupAttack({ S.entity.id }, WallModul));
						FoundOP = true;
					}
				}
			}

			if(FoundOP==false)
			{
				//MISD("FoundOP = FALSE")
				Bro->U->CloseCombi({ S.entity },
					entitiesTOentity(opId, lState.entities.power_slots, lState.entities.token_slots, lState.entities.barrier_modules), A, B); //lState.entities.squads,
				vReturn.push_back(MIS_CommandGroupGoto({ S.entity.id }, capi::to2D(B.position), capi::WalkMode_Normal));
			}
			vIdler.push_back(new MIS_Ideler(S.entity.id,lState.current_tick + Bro->L->IdleOffset));			
		}

	
	MISE;
	return vReturn;
}

bool FireBot::onWall(capi::Entity E)
{
	for (auto A : E.aspects)
		if (A.variant_case == capi::AspectCase::MountBarrier)
			return true;
	return false;
}

capi::EntityId FireBot::behindWall(capi::Entity myUnit, capi::Entity opUnit)
{
	MISS;
	capi::Entity A;
	capi::Entity B;

	std::vector < capi::BarrierModule> opWalls = Bro->U->FilterBarrierModule(opId, lState.entities.barrier_modules);
	//No Walls
	if (opWalls.size() == 0)
	{
		MISEA("#1");
		return 0;
	}

	float UnitToUnit = Bro->U->distance(capi::to2D(myUnit.position), capi::to2D(opUnit.position));
	float UnitToWall = Bro->U->CloseCombi({ myUnit }, entitiesTOentity(opId, opWalls), A, B);
	//Unit is coser then wall
	if (UnitToUnit < UnitToWall)
	{
		MISEA("#2");
		return 0;
	}
	
	//I guess behin the wall
	for (auto W : opWalls)if (W.entity.id == B.id)
	{
		MISEA("#3");
		return W.entity.id;
	}
	
	MISEA("#4");
	return 0;
}

capi::EntityId FireBot::SetOfWallmodul(capi::EntityId E)
{
	for (auto W : lState.entities.barrier_modules)if (W.entity.id == E)return W.set;	
	return 0;
}

bool FireBot::GapInWall(capi::EntityId E)
{
	if (E == 0) return true;

	unsigned int DesWallSec = 0;
	for (auto W : Bro->U->FilterBarrierModule(opId, lState.entities.barrier_modules))
		if (E == W.set && GetAspect(W.entity, "Health") == 0) DesWallSec++;
		
	return DesWallSec >= 2;
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

bool FireBot::OrbOneOK()
{
	for (auto O : entitiesTOentity(myId, lState.entities.token_slots))if (O.id == eMainOrb.id)return true;
	return false;
}

float FireBot::GetSquadHP(capi::EntityId SquadID)
{
	float iReturn = 0;
	for (auto F : lState.entities.figures)	
		if (F.squad_id == SquadID)		
			iReturn += GetAspect(F.entity, "Health");
	
	return iReturn;
}

float FireBot::GetSquadMAXHP(capi::EntityId SquadID)
{
	float iReturn = 0;
	for (auto F : lState.entities.figures)
		if (F.squad_id == SquadID)
			iReturn += GetAspect(F.entity, "MaxHealth");

	return iReturn;
}

float FireBot::GetAspect(capi::Entity E, std::string Asp)
{
	for (auto A : E.aspects)
	{
		if (A.variant_case == capi::AspectCase::Health && Asp == "Health")
			return A.variant_union.health.current_hp;
		if (A.variant_case == capi::AspectCase::Health && Asp == "MaxHealth")
			return A.variant_union.health.cap_current_max;
	}
	return 0;
}


std::vector<capi::Command> FireBot::WellKiller()
{
	MISS;	
	auto vReturn = std::vector<capi::Command>();
	for (auto W : entitiesTOentity(opId, lState.entities.power_slots))
		if (GetAspect(W, "Health") <= 300)
		{
			MISD("FIRE !!!!");
			auto spell = capi::CommandCastSpellGod();
			spell.card_position = EruptionPos;
			// Spot on, maye chaneg to offset so i can hit units
			spell.target = capi::SingleTargetLocation(capi::to2D(W.position));
			vReturn.push_back(capi::Command(spell));
			return vReturn;
		}

	MISE;
	return vReturn;
}

std::vector<capi::Command> FireBot::InstantRepairFunction()
{
	MISS;
	bool inRebpair;

	auto vReturn = std::vector<capi::Command>();
	for (auto E : entitiesTOentity(myId, lState.entities.power_slots, lState.entities.token_slots))
		if (GetAspect(E, "Health") < GetAspect(E, "MaxHealth") - 1)
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