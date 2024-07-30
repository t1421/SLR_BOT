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

void FireBot::ChangeStrategy(Stages _Stage, int _Value)
{
	if (Bro->L->vStrategy.size() > 0) if (Bro->L->vStrategy[0].first == _Stage)return;
	Bro->L->vStrategy.insert(Bro->L->vStrategy.begin(), std::make_pair(_Stage, _Value));
	bStage = true;
}


std::vector<capi::Command>  FireBot::IdleToFight(const capi::GameState& state)
{
	auto vReturn = std::vector<capi::Command>();
	capi::Entity A;
	capi::Entity B;

	for(auto S: Bro->U->FilterSquad(myId, state.entities.squads))
		if (S.entity.job.variant_case == capi::JobCase::Idle)
		{
			//Units Close By
			std::vector<capi::Squad> STemp = Bro->U->SquadsInRadius(opId, state.entities.squads, capi::to2D(S.entity.position), FightRange);
			if (STemp.size() > 0)
			{
				vReturn.push_back(MIS_CommandGroupAttack({ S.entity.id }, STemp[0].entity.id));
			}
			else
			{
				Bro->U->CloseCombi({ S.entity },
					entitiesTOentity(opId, state.entities.power_slots, state.entities.token_slots, state.entities.squads), A, B);
				vReturn.push_back(MIS_CommandGroupGoto({ S.entity.id }, capi::to2D(B.position), capi::WalkMode_Normal));
			}
		}

	return vReturn;
}