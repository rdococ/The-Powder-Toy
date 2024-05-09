#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);
static void create(ELEMENT_CREATE_FUNC_ARGS);

void Element::Element_INPT()
{
	Identifier = "RDOCOC_PT_INPT";
	Name = "INPT";
	Colour = 0x448888_rgb;
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
	HotAir = 0.000f	* CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 1;
	Hardness = 0;
	Weight = 100;

	HeatConduct = 251;
	Description = "Logic input, takes input from LGWR and feeds it to logic gates.";

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
	for (auto rx = -1; rx <= 1; rx++)
	{
		for (auto ry = -1; ry <= 1; ry++)
		{
			if ((!!rx) != (!!ry))
			{
				auto r = pmap[y + ry][x + rx];
				if (!r)
					continue;
				if (TYP(r) == PT_LGWR && parts[ID(r)].tmp > 0)
				{
					parts[i].tmp = 1;
					return 0;
				}
			}
		}
	}

	parts[i].tmp = 0;
	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	if (cpart->tmp > 0)
	{
		*colr = 128;
		*colb = *colg = 255;

		*pixel_mode |= FIRE_BLEND;
		*firea = 32;
		*firer = *colr/2;
		*fireg = *colg/2;
		*fireb = *colb/2;
	}
	return 0;
}