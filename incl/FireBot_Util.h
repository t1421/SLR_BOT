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
	MISD("ADD: " + std::to_string(_Stage) + "(" + std::to_string(_Value) + ") " + SwitchStagesText(_Stage));
	MISD("eStage = " + std::to_string(eStage) + "(" + std::to_string(iStageValue) + ") " + SwitchStagesText(eStage));
	
	if (Bro->L->vStrategy.size() > 0)
	{
		MISD("vStrategy[0]: " + std::to_string(Bro->L->vStrategy[0].first) + "(" + std::to_string(Bro->L->vStrategy[0].second) + ") ");
		if (Bro->L->vStrategy[0].first == _Stage)return;
	}
	Bro->L->vStrategy.insert(Bro->L->vStrategy.begin(), std::make_pair(_Stage, _Value));
	bStage = true;
}

void FireBot::ReplaceStrategy(Stages _Stage, int _Value)
{
	MISD("Replace: " + std::to_string(_Stage) + "(" + std::to_string(_Value) + ") " + SwitchStagesText(_Stage));
	MISD("ReStage = " + std::to_string(eStage) + "(" + std::to_string(iStageValue) + ") " + SwitchStagesText(eStage));

	if (Bro->L->vStrategy.size() > 0)
	{
		MISD("RvStrategy[0]: " + std::to_string(Bro->L->vStrategy[0].first) + "(" + std::to_string(Bro->L->vStrategy[0].second) + ") ");
		Bro->L->vStrategy[0].first = _Stage;
		Bro->L->vStrategy[0].second = _Value;
		eStage = _Stage;
		iStageValue = _Value;
	}
	else
	{
		ChangeStrategy(_Stage, _Value);
	}
}


std::vector<capi::Command>  FireBot::IdleToFight()
{
	auto vReturn = std::vector<capi::Command>();
	capi::Entity A;
	capi::Entity B;

	for(auto S: Bro->U->FilterSquad(myId, lState.entities.squads))
		if (S.entity.job.variant_case == capi::JobCase::Idle)
		{
			//Units Close By
			std::vector<capi::Squad> STemp = Bro->U->SquadsInRadius(opId, lState.entities.squads, capi::to2D(S.entity.position), FightRange);
			if (STemp.size() > 0)
			{
				vReturn.push_back(MIS_CommandGroupAttack({ S.entity.id }, STemp[0].entity.id));
			}
			else
			{
				Bro->U->CloseCombi({ S.entity },
					entitiesTOentity(opId, lState.entities.power_slots, lState.entities.token_slots, lState.entities.squads, lState.entities.barrier_modules), A, B);
				vReturn.push_back(MIS_CommandGroupGoto({ S.entity.id }, capi::to2D(B.position), capi::WalkMode_Normal));
			}
		}

	return vReturn;
}