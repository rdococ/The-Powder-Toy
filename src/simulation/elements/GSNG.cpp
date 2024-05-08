#include "simulation/ElementCommon.h"
#include "simulation/orbitalparts.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_GSNG()
{
	Identifier = "RDOCOC_PT_GSNG";
	Name = "GSNG";
	Colour = 0x202020_rgb;
	MenuVisible = 1;
	MenuSection = SC_NUCLEAR;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.90f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = -0.005f * CFDS;
	Falldown = 2;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = 100;

	HeatConduct = 186;
	Description = "Gravitational singularity. Forms a ball, sucks things in with gravity, produces Hawking radiation.";

	Properties = TYPE_LIQUID;

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
	if (sim->rng.chance(1, 1000)) {
		sim->part_change_type(i, x, y, PT_ELEC);
		float spd = sim->rng.uniform01() * 1.0f + 2.0f;
		parts[i].vx = -sim->gravx[(y / CELL) * XCELLS + (x / CELL)] * spd;
		parts[i].vy = -sim->gravy[(y / CELL) * XCELLS + (x / CELL)] * spd;
		return 0;
	}
	if (parts[i].tmp)
		sim->gravmap[(y/CELL)*XCELLS+(x/CELL)] += restrict_flt(0.001f*parts[i].tmp, 0.1f, 51.2f);
	else
		sim->gravmap[(y/CELL)*XCELLS+(x/CELL)] += 0.1f;

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	*firea = 255;
	*firer = *colr;
	*fireg = *colg;
	*fireb = *colb;
	*pixel_mode = FIRE_BLEND;
	return 0;
}