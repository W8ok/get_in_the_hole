// world.h
#pragma once

#include "types.h"
#include "utils.h"

#define TILE(map, row, col) (map)->data[(row) * (map)->size.x + (col)]

typedef enum {
	MAP_TUTORIAL1,
	MAP_TUTORIAL2,
	MAP_TUTORIAL3,
	MAP_COUNT,
} MapID;

typedef struct Map {
	const u8* data;
	u8_2 size;
} Map;

typedef struct WorldContext {
	Map maps[MAP_COUNT];
	f32 tile_size;
	MapID current_map;
} WorldContext;

void world_init(WorldContext* wc);
