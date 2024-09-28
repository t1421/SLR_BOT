#define DF_Debug

#include "../incl/FireBot.h"
#include "../incl/CardBaseSMJ.h"
#include "../incl/Util.h"
#include "../incl/LOAD.h"


void FireBot::CleanUpRejectedComamandChecklist()
{
	MISS;
	for (std::vector<MIS_RejectCheck>::iterator it = RejectedComamandChecklist.begin(); it != RejectedComamandChecklist.end();)
		if ((*it).rejected == false)it = RejectedComamandChecklist.erase(it);
		else  ++it;
	MISE;
}

std::vector<capi::Command> FireBot::Handel_CardRejected_ProduceSquad(capi::Command cIN) //, std::vector<capi::Command>& addHere)
{
	MISS;

	std::vector<capi::Command> vReturn;

	for (unsigned int i = 0; i < RejectedComamandChecklist.size(); i++)
	{
		if (RejectedComamandChecklist[i].lastCommand.variant_case == cIN.variant_case)
		{
			if (RejectedComamandChecklist[i].lastCommand.variant_union.produce_squad.card_position == cIN.variant_union.produce_squad.card_position
				&& RejectedComamandChecklist[i].lastCommand.variant_union.produce_squad.xy.x == cIN.variant_union.produce_squad.xy.x
				&& RejectedComamandChecklist[i].lastCommand.variant_union.produce_squad.xy.y == cIN.variant_union.produce_squad.xy.y)
			{
				RejectedComamandChecklist[i].rejected = true;
				RejectedComamandChecklist[i].lastCommand = RejectedComamandChecklist[i].orgCommand;
				switch (RejectedComamandChecklist[i].retry)
				{
				case 0:
					RejectedComamandChecklist[i].lastCommand.variant_union.produce_squad.xy.x += 15;
					break;
				case 1:
					RejectedComamandChecklist[i].lastCommand.variant_union.produce_squad.xy.y += 15;
					break;
				case 2:
					RejectedComamandChecklist[i].lastCommand.variant_union.produce_squad.xy.x -= 15;
					break;
				case 3:
					RejectedComamandChecklist[i].lastCommand.variant_union.produce_squad.xy.y -= 15;
					break;
				case 4:
					RejectedComamandChecklist[i].lastCommand.variant_union.produce_squad.xy.x += 15;
					RejectedComamandChecklist[i].lastCommand.variant_union.produce_squad.xy.y += 15;
					break;
				case 5:
					RejectedComamandChecklist[i].lastCommand.variant_union.produce_squad.xy.x += 15;
					RejectedComamandChecklist[i].lastCommand.variant_union.produce_squad.xy.y -= 15;
					break;
				case 6:
					RejectedComamandChecklist[i].lastCommand.variant_union.produce_squad.xy.x -= 15;
					RejectedComamandChecklist[i].lastCommand.variant_union.produce_squad.xy.y -= 15;
					break;
				case 7:
					RejectedComamandChecklist[i].lastCommand.variant_union.produce_squad.xy.x -= 15;
					RejectedComamandChecklist[i].lastCommand.variant_union.produce_squad.xy.y += 15;
					break;
				default:
					RejectedComamandChecklist[i].rejected = false;
					break;
				}
				RejectedComamandChecklist[i].retry++;

				if (RejectedComamandChecklist[i].rejected)
				{
					vReturn.push_back(RejectedComamandChecklist[i].lastCommand);
#ifdef MIS_DEBUG
					if(Bro->L->DrawRejected)vReturn.push_back(MIS_Ping_Attention(RejectedComamandChecklist[i].lastCommand.variant_union.produce_squad.xy));
#endif // MIS_DEBUG	
				}
			}
		}

	}

	MISE;
	return vReturn;
}