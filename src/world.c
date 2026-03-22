// world.c
#include "world.h"

// TODO
// Make a python script to make these for me
static const u8 tutorial1[5][5] = {
	{1,1,1,1,1},
	{1,0,0,0,1},
	{1,0,0,0,1},
	{1,0,0,0,1},
	{1,1,1,1,1},
};

static const u8 tutorial2[7][7] = {
	{1,1,1,1,1,1,1},
	{1,1,1,0,1,1,1},
	{1,1,0,0,0,1,1},
	{1,0,0,0,0,0,1},
	{1,1,0,0,0,1,1},
	{1,1,1,0,1,1,1},
	{1,1,1,1,1,1,1},
};

static const u8 tutorial3[9][9] = {
	{1,1,1,1,1,1,1,1,1},
	{1,0,0,0,1,0,0,0,1},
	{1,0,1,0,0,0,1,0,1},
	{1,0,0,0,1,0,0,0,1},
	{1,1,0,1,1,1,0,1,1},
	{1,0,0,0,1,0,0,0,1},
	{1,0,1,0,0,0,1,0,1},
	{1,0,0,0,1,0,0,0,1},
	{1,1,1,1,1,1,1,1,1},
};

#define MAP_INIT(name) \
	(Map){ \
		.data = (const u8*)name, \
		.size = { \
			.x = sizeof(name[0]) / sizeof(name[0][0]), \
			.y = sizeof(name) / sizeof(name[0]) \
		} \
	}

void world_init(WorldContext* wc)
{
	wc->maps[MAP_TUTORIAL1] = MAP_INIT(tutorial1);
	wc->maps[MAP_TUTORIAL2] = MAP_INIT(tutorial2);
	wc->maps[MAP_TUTORIAL3] = MAP_INIT(tutorial3);

	wc->tile_size = 32.0f;
}

#undef MAP_DATA
