#define DF_Debug

#include "../incl/FireBot.h"
#include "../incl/CardBaseSMJ.h"
#include "../incl/Util.h"
#include "../incl/LOAD.h"

bool FireBot::CalGlobalBattleTable(const capi::GameState& state)
{
	MISS;
	std::vector<capi::Squad> mySquads;
	std::vector<capi::Squad> opSquads;

	for (auto S : state.entities.squads)
	{
		if (S.entity.player_entity_id == myId)mySquads.push_back(S);
		else if (S.entity.player_entity_id == opId)opSquads.push_back(S);
	}

	myBT = CalcBattleTable(mySquads);
	opBT = CalcBattleTable(opSquads);

	MISE;
	return true;
}

BattleTable FireBot::CalcBattleTable(std::vector<capi::Squad> squads)
{
	MISS;
	BattleTable BTreturn;
	BTreturn.Init();
	Card TempCard;
	for (auto S : squads)
	{
		TempCard = Bro->J->CardFromJson(S.card_id % 1000000);
		if (TempCard.offenseType < 0)TempCard.offenseType = 4; //fall back;
		if (TempCard.defenseType < 0)TempCard.defenseType = 4; //fall back;
		BTreturn.SizeCounter[TempCard.defenseType][TempCard.offenseType] += TempCard.health;

		if (TempCard.movementType == 1)BTreturn.Flyer++;
	}
	MISE;
	return BTreturn;
}

void FireBot::EchoBattleTable(BattleTable BT)
{
	MISS;
	std::stringstream ssLine;
	MISERROR("SIZE ->  #      S #      M #      L #     XL #");
	MISERROR("COUNTER  #####################################");
	for (unsigned int iCounter = 0; iCounter < 5; iCounter++)
	{
		ssLine.str("");
		if (iCounter == 0)     ssLine << "       S #";
		else if (iCounter == 1)ssLine << "       M #";
		else if (iCounter == 2)ssLine << "       L #";
		else if (iCounter == 3)ssLine << "      Xl #";
		else if (iCounter == 4)ssLine << "       ? #";

		for (unsigned int iSize = 0; iSize < 4; iSize++)
		{
			ssLine << std::fixed << std::setw(7) << std::setfill(' ') << std::setprecision(0) << BT.SizeCounter[iSize][iCounter] << " #";
		}
		MISERROR(ssLine.str());
	}
	MISE;
}

bool FireBot::MoreUnitsNeeded(BattleTable myBT, BattleTable opBT)
{
	std::vector<int> temp;
	return MoreUnitsNeeded(myBT, opBT,temp);
}

bool FireBot::MoreUnitsNeeded(BattleTable myBT, BattleTable opBT, std::vector<int>& PowerLevel)
{
	MISS;
	
	PowerLevel.clear();

	for (unsigned int iSize = 0; iSize < 4; iSize++)
		PowerLevel.push_back(myBT.SumCounter(iSize) - opBT.SumSize(iSize));

	int total = 0;
	for (auto i : PowerLevel)
	{
		total += i;
		if (i < Bro->L->MoreUnitsPerType)
		{
			//MISEA("More stuff");
			return true;
		}
	}

	if (total < Bro->L->MoreUnitsGlobal)
	{
		//MISEA("More stuff2");
		return true;
	}

	//Im way a head
	if(lState.players[imyPlayerIDX].power > lState.players[iopPlayerIDX].power * 1.5)
	{
		//MISEA("More stuff3");
		return true;
	}

	//Im swimming in energy
	if (lState.players[imyPlayerIDX].power > Bro->L->ToMutchEnergy)
	{
		//MISEA("More stuff4");
		return true;
	}

	MISE;
	return false;
}