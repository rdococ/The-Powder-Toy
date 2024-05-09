#include "simulation/ElementCommon.h"
#include "simulation/orbitalparts.h"
#include "simulation/Air.h"

static void create(ELEMENT_CREATE_FUNC_ARGS);
static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_STAR()
{
	Identifier = "RDOCOC_PT_STAR";
	Name = "STAR";
	Colour = 0xFF8800_rgb;
	MenuVisible = 1;
	MenuSection = SC_NUCLEAR;
	Enabled = 1;

	Advection = 0.20f;
	AirDrag = 0.10f * CFDS;
	AirLoss = 1.00f;
	Loss = 1.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.00f * CFDS;
	Falldown = 2;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = 30;

	HeatConduct = 186;
	Description = "Solar plasma. Produces immense pressure and gravity.";

	Properties = TYPE_LIQUID;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = 255.0f;
	HighPressureTransition = PT_GSNG;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Create = &create;
	Update = &update;
	Graphics = &graphics;
}

static void create(ELEMENT_CREATE_FUNC_ARGS)
{
	sim->parts[i].temp = sim->rng.between(5000, 6000);
	sim->parts[i].tmp2 = sim->rng.between(5000, 6000);
}

static int update(UPDATE_FUNC_ARGS)
{
	sim->gravmap[(y/CELL)*XCELLS+(x/CELL)] += 0.04f;

	for (auto rx = -1; rx <= 1; rx++)
	{
		for (auto ry = -1; ry <= 1; ry++)
		{
			if (rx || ry)
			{
				auto r = pmap[y + ry][x + rx];
				if (!r) continue;
				if (TYP(r) == PT_STAR)
				{
					parts[i].tmp2 = (parts[i].tmp2 + parts[ID(r)].tmp2) / 2;
					parts[ID(r)].tmp2 = parts[i].tmp2;
				}
			}
		}
	}

	float hygnLoss = floor(parts[i].temp / 2000.0f);

	if (parts[i].tmp2 >= hygnLoss)
	{
		parts[i].tmp2 -= floor(parts[i].temp / 2000.0f);
	}
	else if (parts[i].temp >= 3000.0f && sim->rng.chance(1, 10))
	{
		for (auto rx = -1; rx <= 1; rx++)
		{
			for (auto ry = -1; ry <= 1; ry++)
			{
				if (rx || ry)
				{
					auto r = pmap[y + ry][x + rx];
					if (!r)
					{	
						parts[i].temp *= 0.8f;
						auto np = sim->create_part(-1, x + rx, y + ry, PT_STAR);
						if (np < 0) continue;
						parts[np].temp = parts[i].temp;
						parts[np].tmp2 = parts[i].tmp;
					}
				}
			}
		}
	}
	if (parts[i].tmp2 <= 0)
	{
		sim->pv[y / CELL][x / CELL] += 0.04f;
	}
	else
	{
		sim->pv[y / CELL][x / CELL] += 0.01f;
	}

	int orbd[4] = { 0, 0, 0, 0 };	//Orbital distances
	int orbl[4] = { 0, 0, 0, 0 };	//Orbital locations
	if (!sim->parts[i].life) parts[i].life = sim->rng.gen();
	if (!sim->parts[i].ctype) parts[i].ctype = sim->rng.gen();
	orbitalparts_get(parts[i].life, parts[i].ctype, orbd, orbl);
	for (auto r = 0; r < 4; r++)
	{
		if (orbd[r] < 254)
		{
			orbd[r] += 16;
			if (orbd[r] > 254) {
				orbd[r] = 0;
				orbl[r] = sim->rng.between(0, 254);
			}
			else
			{
				orbl[r] += 1;
				orbl[r] = orbl[r] % 255;
			}
		}
		else
		{
			orbd[r] = 0;
			orbl[r] = sim->rng.between(0, 254);
		}
	}
	orbitalparts_set(&parts[i].life, &parts[i].ctype, orbd, orbl);

	if (abs(parts[i].vx) > 5.0f || abs(parts[i].vy) > 5.0f)
	{
		sim->kill_part(i);
		sim->create_part(-1, x, y, PT_FIRE);
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	int temp = cpart->temp;
	*colr = *colg = *colb = 0;

	if (temp >= 0.0f and temp <= 2500.0f) /* black to red */
	{
		*colr = temp / 2500.0f * 255.0f;
	}
	if (temp >= 2500.0f and temp <= 5000.0f) /* red to yellow */
	{
		*colr = 255;
		*colg = (temp - 2500.0f) / 2500.0f * 255.0f;
	}
	if (temp >= 5000.0f and temp <= 6000.0f) /* yellow to white */
	{
		*colr = *colg = 255;
		*colb = (temp - 5000.0f) / 1000.0f * 255.0f;
	}
	if (temp >= 6000.0f) /* white to blue */
	{
		*colr = *colg = 255 - ((temp - 6000.0f) / 5000.0f * 196.0f);
		*colb = 255;
	}

	if (*colr < 0)
		*colr = 0;
	if (*colr > 255)
		*colr = 255;
	if (*colg < 0)
		*colg = 0;
	if (*colg > 255)
		*colg = 255;
	if (*colb < 0)
		*colb = 0;
	if (*colb > 255)
		*colb = 255;

	*firea = 64;
	*firer = *colr;
	*fireg = *colg;
	*fireb = *colb;

	*pixel_mode |= FIRE_BLEND | EFFECT_GRAVOUT;
	return 0;
}