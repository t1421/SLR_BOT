#define DF_Debug

#include "../incl/FireBot.h"
#include "../incl/CardBaseSMJ.h"
#include "../incl/Util.h"
#include "../incl/LOAD.h"

void FireBot::FindAvoidArea()
{
	MISS;

	for (auto O : lState.entities.ability_world_objects)
	{
		//Only OP objects and only New ons
		if (O.entity.player_entity_id.value() != opId
			|| O.entity.id <= MaxAvoidID)continue;

		MaxAvoidID = O.entity.id;

		for (auto E : O.entity.effects)
		{
			if (E.id % 1000000 == 1620)
			{
				MISD("Add Glyth til " + std::to_string(E.end_tick.value()));
				vAvoid.push_back(new MIS_AvoidArea(
					E.end_tick.value(),
					capi::to2D(O.entity.position),
					E.specific.variant_union.spell_on_entity_nearby.radius));
			}
			if (E.id % 1000000 == 1694)
			{
				MISD("Add Root til " + std::to_string(E.end_tick.value()));
				vAvoid.push_back(new MIS_AvoidArea(
					E.end_tick.value(),
					capi::to2D(O.entity.position),
					E.specific.variant_union.aura.radius));
			}
			if (E.id % 1000000 == 290)
			{
				MISD("Add Could snap til " + std::to_string(E.end_tick.value()));
				vAvoid.push_back(new MIS_AvoidArea(
					E.end_tick.value(),
					capi::to2D(O.entity.position),
					20));
			}
			if (E.id % 1000000 == 1471 ||
				E.id % 1000000 == 4427)
			{
				MISD("Add Aura of Coruption til " + std::to_string(E.end_tick.value()));
				vAvoid.push_back(new MIS_AvoidArea(
					E.end_tick.value(),
					capi::to2D(O.entity.position),
					30));
			}
			if (E.id % 1000000 == 1006)
			{
				MISD("Add Paralyse til " + std::to_string(E.end_tick.value()));
				vAvoid.push_back(new MIS_AvoidArea(
					E.end_tick.value(),
					capi::to2D(O.entity.position),
					20));
			}

			if (E.id % 1000000 == 11003)
			{
				MISD("Add Bandid Minfield til " + std::to_string(E.end_tick.value()));

				capi::Position2D EndPos;
				EndPos.x = capi::to2D(O.entity.position).x +
					E.specific.variant_union.moving_interval_cast.direction_step.x * 100;
				EndPos.y = capi::to2D(O.entity.position).y +
					E.specific.variant_union.moving_interval_cast.direction_step.y * 100;

				for (unsigned int i = 0; i < 4; i++)
				{
					vAvoid.push_back(new MIS_AvoidArea(
						E.end_tick.value(),
						Bro->U->Offseter(capi::to2D(O.entity.position),
							EndPos, 6 * i),
						10));
				}

			}
			if (E.id % 1000000 == 1740)
			{
				MISD("Add Wild Fire til " + std::to_string(E.end_tick.value()));

				capi::Position2D EndPos;
				EndPos.x = capi::to2D(O.entity.position).x +
					E.specific.variant_union.moving_interval_cast.direction_step.x * 100;
				EndPos.y = capi::to2D(O.entity.position).y +
					E.specific.variant_union.moving_interval_cast.direction_step.y * 100;

				int iRange = abs(Bro->U->distance(capi::to2D(O.entity.position),
					EndPos));
				if (iRange <= 15)iRange = 15;

				for (unsigned int i = 0; i < 4; i++)
				{
					for (int j = -1; j < 2; j++)
					{
						vAvoid.push_back(new MIS_AvoidArea( //Tick1 L
							E.end_tick.value(),
							Bro->U->Offseter(
								capi::to2D(O.entity.position),
								EndPos,
								iRange / 4.0f * i,
								5.0f * j),
							5));
					}
				}
			}


			if (E.id % 1000000 == 1775)
			{
				MISD("Add Hurrican " + std::to_string(E.end_tick.value()));

				for (unsigned int i = 0; i < 8; i++)
				{
					vAvoid.push_back(new MIS_AvoidArea(
						E.end_tick.value(),
						Bro->U->Offseter(E.specific.variant_union.damage_area.shape.variant_union.wide_line.start,
							E.specific.variant_union.damage_area.shape.variant_union.wide_line.end,
							50 / 8 * i),
						5));
				}

			}

		}
	}

	MISE;
}

void FireBot::RemoveFromMIS_AvoidArea(capi::Tick curTick)
{
	MISS;
	for (std::vector<MIS_AvoidArea*>::iterator it = vAvoid.begin(); it != vAvoid.end();)
	{
		if ((*it)->endTick <= curTick)
		{
			it = vAvoid.erase(it);
		}
		else  ++it;
	}
	MISE;
}

std::vector<capi::Command> FireBot::MoveUnitsAway()
{
	MISS;

	auto vReturn = std::vector<capi::Command>();

	for (auto A : vAvoid)
	{
		for (auto EE : Bro->U->pointsInRadius(entitiesTOentity(myId, lState.entities.squads), A->pos, A->radius * 1.1))
		{
			MISD("MOVE Units");
			auto move = capi::CommandGroupGoto();
			move.squads = { EE.id };
			move.positions = { (Bro->U->Offseter(capi::to2D(EE.position), A->pos, A->radius * -1 * 1.2)) };
			move.walk_mode = capi::WalkMode_Normal;
			vReturn.push_back(capi::Command(move));
		}
	}

	MISE;
	return vReturn;
}

