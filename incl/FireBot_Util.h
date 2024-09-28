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
			std::vector<capi::Squad> SOP = Bro->U->SquadsInRadius(opId, lState.entities.squads, capi::to2D(S.entity.position), FightRange);
			//for (auto S : STemp)
			for(unsigned int i = 0; i < SOP.size() && OnWall == false; i++)
			{
				//Do not Atack stuff on walls				
				for (unsigned int j = 0; j < SOP[i].entity.aspects.size() && OnWall == false; j++)
					if (SOP[i].entity.aspects[j].variant_case == capi::AspectCase::MountBarrier)
						OnWall = SOP[i].entity.aspects[j].variant_union.mount_barrier.state.variant_case == capi::MountStateCase::MountedSquad;
							
							
				
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