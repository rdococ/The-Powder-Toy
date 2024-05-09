#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);
static void create(ELEMENT_CREATE_FUNC_ARGS);

void Element::Element_OR()
{
	Identifier = "RDOCOC_PT_OR";
	Name = "OR";
	Colour = 0x44FF44_rgb;
	MenuVisible = 1;
	MenuSection = SC_ELEC;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.90f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 1;
	Hardness = 0;
	Weight = 100;

	HeatConduct = 251;
	Description = "Logic OR gate, takes input from INPT and outputs to LGWR.";

	Properties = TYPE_SOLID | PROP_LIFE_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &update;
	Graphics = &graphics;
}

static int update(UPDATE_FUNC_ARGS)
{
	if (parts[i].life > 0) return 0;

	bool signal = false;
	int pos = 0;
	std::vector<int> queue;
	std::vector<int> wires;

	queue.push_back(i);

	while (queue.size() > pos)
	{
		int id = queue[pos];
		int type = parts[id].type;

		if (type == PT_OR && parts[id].life < 1)
		{
			wires.push_back(id);
			parts[id].life = 1;

			int x = floor(parts[id].x);
			int y = floor(parts[id].y);

			if (pmap[y][x + 1]) queue.push_back(ID(pmap[y][x + 1]));
			if (pmap[y][x - 1]) queue.push_back(ID(pmap[y][x - 1]));
			if (pmap[y + 1][x]) queue.push_back(ID(pmap[y + 1][x]));
			if (pmap[y - 1][x]) queue.push_back(ID(pmap[y - 1][x]));
		}
		else if (type == PT_INPT && parts[id].tmp > 0)
		{
			signal = true;
		}
		else if (type == PT_NSCN)
		{
			wires.push_back(id);
		}

		pos += 1;
	}

	for (int i = 0; i < wires.size(); i++)
	{
		int id = wires[i];
		if (parts[id].type == PT_NSCN)
		{
			if (signal)
			{
				parts[id].ctype = PT_NSCN;
				sim->part_change_type(id, parts[id].x, parts[id].y, PT_SPRK);
			}
		}
		else if (signal)
			parts[id].tmp = 1;
		else
			parts[id].tmp = 0;
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	if (cpart->tmp > 0)
	{
		*colr = *colb = 128;

		*pixel_mode |= FIRE_BLEND;
		*firea = 32;
		*firer = *colr / 2;
		*fireg = *colg / 2;
		*fireb = *colb / 2;
	}
	return 0;
}