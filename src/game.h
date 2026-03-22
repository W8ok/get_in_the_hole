// game.h
#pragma once

#include "types.h"
#include "utils.h"
#include "input.h"
#include "world.h"

typedef struct Player {
	// I named him Gayboy
	struct {
		u8_2* data;	
		u16 count;
		u16 capacity;
	} pos_list;

	u8_2 current_pos;
	u8_2 spawn_pos;
	u8 health;
} Player;

typedef struct Ghost {
	struct {
		u8_2* data;
		u16 count;
		u16 capacity;
	} pos_list;

	u8_2 current_pos;
	u16 move_cap;
	bool active;
} Ghost;

typedef struct Ghosts {
	Ghost* data;
	u16 count;
	u16 capacity;
} Ghosts;

typedef struct {
	u8_2 tile;
	bool occupied;
} SpawnableTile;

typedef struct GameContext {
	const WorldContext* wc;

	Player player;
	Ghosts ghost;

	struct {
		SpawnableTile* data;
		u16 count;
		u16 capacity;
		u16 last_occupied;
	} spawn_tiles;

	u8_2 hole_pos;
	u16 score;

	bool hole_spawned;
	bool player_spawned;
	bool ghosts_spawned;
	bool game_over;
} GameContext;

bool game_main(GameContext* gc);
bool game_init(GameContext* gc);
void game_quit(GameContext* gc);
