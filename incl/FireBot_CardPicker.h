#define DF_Debug

#include "../incl/FireBot.h"
#include "../incl/CardBaseSMJ.h"
#include "../incl/Util.h"
#include "../incl/LOAD.h"

int FireBot::CardPicker(unsigned int opSize, unsigned int opCounter, CardPickCrit Crit) // , unsigned int Tier)
{
	MISS;

	int tier = entitiesTOentity(myId, lState.entities.token_slots).size();
	if (tier == 2 && TierReadyTick > lState.current_tick ) tier = 1;

	for (unsigned int i = 0; i < SMJDeck.size(); i++)
	{
		if ((SMJDeck[i].offenseType == opSize || opSize == 99)
			&& (SMJDeck[i].defenseType != opCounter || opCounter == 99)	)
		{
			//Skip flyer for the moment since you cant spawrn new stuf neer it
			if(SMJDeck[i].officialCardIds== 703)continue;


			//If not in my Tier then skip
			if(Crit != IgnorTier && SMJDeck[i].orbsTotal < tier) continue; 

			switch (Crit)
			{
			case Swift:
				for (Ability A : SMJDeck[i].abilities)
					if (A.abilityIdentifier == "Swift")
					{
						MISEA("Swift: " + SMJDeck[i].cardName);
						return i;
					}
				break;
			case Archer:
				if (SMJDeck[i].unitClass == "Archer")
				{
					MISEA("Archer: " + SMJDeck[i].cardName);
					return i;
				}
				break;
			case Siege:
				for (Ability A : SMJDeck[i].abilities)
					if (A.abilityIdentifier == "Siege")
					{
						MISEA("Siege: " + SMJDeck[i].cardName);
						return i;
					}
				break;
				
			case NotS:
				if (SMJDeck[i].defenseType == 0)break;
			case NotM:
				if (SMJDeck[i].defenseType == 1)break;
			case NotL:
				if (SMJDeck[i].defenseType == 2)break;

			case None:
			default:
				MISEA("Default: " + SMJDeck[i].cardName)
					return i;
			}

		}
	}

	MISEA("No Card");
	return -1;
}

int FireBot::CardPickerFromBT(BattleTable BT, CardPickCrit Crit) // , unsigned int Tier)
{
	MISS;
	int iReturn = -1;
	unsigned int MaxSize = 0;
	unsigned int MaxCounter = 0;
	unsigned int MaxHealth = 0;

	for (unsigned int iCounter = 0; iCounter < 5; iCounter++)
		for (unsigned int iSize = 0; iSize < 4; iSize++)
			if (MaxHealth < BT.SizeCounter[iSize][iCounter])
			{
				MaxSize = iSize;
				MaxCounter = iCounter;
				MaxHealth = BT.SizeCounter[iSize][iCounter];
			}

	//Maybe check my own Battle Tabble if i have egnaut units of that type???

	// Stuff vs Flyer
	if (BT.Flyer && Crit == None)Crit = Archer;

	if ((iReturn = CardPicker(MaxSize, MaxCounter, Crit)) == -1)    //Perfect Counter
		if ((iReturn = CardPicker(MaxSize, 9, Crit)) == -1)   //Counter of any Size
			if ((iReturn = CardPicker(MaxSize, MaxCounter, None)) == -1)    //Perfect Counter, not Crits
				if ((iReturn = CardPicker(MaxSize, 9, None)) == -1)   //Counter of any Size, not Crits
					iReturn = -1; // I dont have a counter card???

	// If size is XL do XYZ


	if (iReturn == -1)
	{
		unsigned int iAvoidSize = MaxSize;
		MaxSize = 0;
		MaxCounter = 0;
		MaxHealth = 0;
		for (unsigned int iCounter = 0; iCounter < 5; iCounter++)
			for (unsigned int iSize = 0; iSize < 4; iSize++)
				if (MaxHealth < BT.SizeCounter[iSize][iCounter] && iAvoidSize != iSize)
				{
					MaxSize = iSize;
					MaxCounter = iCounter;
					MaxHealth = BT.SizeCounter[iSize][iCounter];
				}
		if ((iReturn = CardPicker(MaxSize, MaxCounter, Crit)) == -1)    //Perfect Counter
			if ((iReturn = CardPicker(MaxSize, 9, Crit)) == -1)   //Counter of any Size
				if ((iReturn = CardPicker(MaxSize, MaxCounter, None)) == -1)    //Perfect Counter, not Crits
					if ((iReturn = CardPicker(MaxSize, 9, None)) == -1)   //Counter of any Size, not Crits
						iReturn = -1; // I dont have a counter card???
	}

	//Still nothing -> try with any Tier
	if (iReturn == -1)
	{
		if ((iReturn = CardPicker(MaxSize, MaxCounter, Crit)) == -1)    //Perfect Counter
			if ((iReturn = CardPicker(MaxSize, 9, Crit)) == -1)   //Counter of any Size
				if ((iReturn = CardPicker(MaxSize, MaxCounter, IgnorTier)) == -1)    //Perfect Counter, not Crits
					if ((iReturn = CardPicker(MaxSize, 9, IgnorTier)) == -1)   //Counter of any Size, not Crits
						iReturn = -1; // I dont have a counter card???
	}


	if (iReturn == -1)iReturn = 0; // Play card 1
	MISE;
	return iReturn;
}
