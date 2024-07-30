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
