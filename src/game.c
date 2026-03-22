// game.c
#include "game.h"

static inline i32 _get_random_int(i32 min, i32 max)
{
	if (min >= max)
		ERROR_RETURN("Invalid input");

	i32 result = min + SDL_rand(max - min + 1);
	return result;
}

static inline bool _player_move(GameContext* gc)
{
	u8_2 new_pos = gc->player.current_pos;

	switch (input.dir)
	{
		case DIR_NONE:	return false;
		case DIR_UP:	new_pos.y--;	break;
		case DIR_DOWN:	new_pos.y++;	break;
		case DIR_LEFT:	new_pos.x--;	break;
		case DIR_RIGHT:	new_pos.x++;	break;
	}

	// Check World Bounds
	// Relying on integer overflow
	if (new_pos.x >= gc->wc->maps[gc->wc->current_map].size.x
			|| new_pos.y >= gc->wc->maps[gc->wc->current_map].size.y)
		return false;

	// Check if tile is walkable
	const Map* map = &gc->wc->maps[gc->wc->current_map];
	u8 tile_type = TILE(map, new_pos.y, new_pos.x);
	if (tile_type != 0)  // Wall or other non-walkable tile
		return false;

	DA_APPEND(gc->player.pos_list, new_pos);
	gc->player.current_pos = new_pos;
	LOG_INFO("Player position: (%d, %d)", gc->player.current_pos.x, gc->player.current_pos.y);

	return true;
}

static inline void _ghost_move(GameContext* gc)
{
	u16 current_move = gc->player.pos_list.count;
	for (u16 i = 0; i < gc->ghost.count; i++)
	{
		if (current_move > gc->ghost.data[i].move_cap)
			continue;

		if (!gc->ghost.data[i].active)
			continue;

		gc->ghost.data[i].current_pos = gc->ghost.data[i].pos_list.data[current_move-1];

		LOG_INFO("Ghost%d position: (%d, %d)", i, gc->ghost.data[i].current_pos.x, gc->ghost.data[i].current_pos.y);
	}
}

static inline bool _spawn_ghost(GameContext* gc)
{
	Ghost new_ghost = (Ghost){
		.pos_list = { 0 },
		.current_pos = gc->player.spawn_pos,
		.move_cap = gc->player.pos_list.count,
		.active = true,
	};
	DA_APPEND(gc->ghost, new_ghost);

	// memcpy is way faster
	size_t ghost_idx = gc->ghost.count - 1;
	LOG_DEBUG("Spawning ghost %zu, copying %d positions", ghost_idx, gc->player.pos_list.count + 1);
	for (u16 i = 0; i < gc->player.pos_list.count; i++)
	{
		DA_APPEND(gc->ghost.data[ghost_idx].pos_list, gc->player.pos_list.data[i]);
		LOG_DEBUG("player[%d] = (%d, %d)", i, gc->player.pos_list.data[i].x, gc->player.pos_list.data[i].y);
	}

	gc->ghosts_spawned = true;
	LOG_INFO("Ghost%zu spawn position: (%d, %d)", ghost_idx, gc->ghost.data[ghost_idx].current_pos.x, gc->ghost.data[ghost_idx].current_pos.y);

	return true;
}

static u8_2 find_valid_spawn_tile(GameContext* gc)
{
	u32 start_time = (u32)SDL_GetTicksNS();
	for (u16 i = gc->spawn_tiles.last_occupied; i < gc->spawn_tiles.count; i++) {
		u8_2 tile = gc->spawn_tiles.data[i].tile;
		if (!gc->spawn_tiles.data[i].occupied)
		{
			gc->spawn_tiles.last_occupied = i;
			gc->spawn_tiles.data[i].occupied = true;
			u32 end_time = (u32)SDL_GetTicksNS();
			LOG_DEBUG("Found spawn tile (%d, %d) after %d ns (checked %d tiles)", tile.x, tile.y, end_time - start_time, i - gc->spawn_tiles.last_occupied + 1);
			return tile;
		}
	}
	ERROR_EXIT("No available spawn positions");
}

static inline bool _spawn_player(GameContext* gc)
{
	gc->player.current_pos = find_valid_spawn_tile(gc);
	gc->player_spawned = true;
	gc->player.spawn_pos = gc->player.current_pos;
	gc->player.pos_list.count = 0;

	DA_APPEND(gc->player.pos_list, gc->player.spawn_pos);
	LOG_INFO("Player spawn position: (%d, %d)", gc->player.spawn_pos.x, gc->player.spawn_pos.y);

	return true;
}

static inline bool _spawn_hole(GameContext* gc)
{
	gc->hole_pos = find_valid_spawn_tile(gc);
	gc->hole_spawned = true;

	LOG_INFO("Hole spawn position: (%d, %d)", gc->hole_pos.x, gc->hole_pos.y);
	return true;
}

static inline bool _is_player_on_hole(GameContext* gc)
{
	return gc->player.current_pos.x == gc->hole_pos.x
		&& gc->player.current_pos.y == gc->hole_pos.y;
}

static inline bool _ghost_collision(GameContext* gc)
{
	// Could add an invincibility frame... just saying
	for (u16 i = 0; i < gc->ghost.count; i++)
	{
		if (gc->ghost.data[i].current_pos.x == gc->player.current_pos.x
				&&	gc->ghost.data[i].current_pos.y == gc->player.current_pos.y)
		{
			LOG_INFO("Hit at (%d, %d)", gc->player.current_pos.x, gc->player.current_pos.y);
			return true;
		}
	}
	return false;
}

static inline bool _score(GameContext* gc)
{
	gc->score++;
	gc->hole_spawned = false;
	gc->player_spawned = false;
	gc->ghosts_spawned = false;

	// Reset ghosts
	for (u16 i = 0; i < gc->ghost.count; i++)
	{
		gc->ghost.data[i].current_pos = gc->ghost.data[i].pos_list.data[0];
		gc->ghost.data[i].active = true;
	}


	return true;
}

static inline void _shuffle_tiles(GameContext* gc)
{
	// Thank god for the internet
	for (u16 i = gc->spawn_tiles.count-1; i > 0; i--)
	{
		u16 j = _get_random_int(0, i);
		SpawnableTile temp = gc->spawn_tiles.data[i];
		gc->spawn_tiles.data[i] =  gc->spawn_tiles.data[j];
		gc->spawn_tiles.data[j] = temp;
	}

	for (u16 i = 0; i < gc->spawn_tiles.count; i++)
    LOG_DEBUG("free_tiles[%d] = (%d, %d) occupied=%d", i, gc->spawn_tiles.data[i].tile.x, gc->spawn_tiles.data[i].tile.y, gc->spawn_tiles.data[i].occupied);
}

static inline bool _generate_world(GameContext* gc)
{
	LOG_INFO("World Bounds: (%d, %d)", gc->wc->maps[gc->wc->current_map].size.x, gc->wc->maps[gc->wc->current_map].size.y);

	// Pre shuffle tiles for future randomized spawn locations
	//
	// Push all walkable tiles to an array
	// Randomize their locations

	if (gc->spawn_tiles.data)
		SDL_free(gc->spawn_tiles.data);
	gc->spawn_tiles.data = NULL;

	gc->spawn_tiles.count = 0;
	gc->spawn_tiles.capacity = 0;
	gc->spawn_tiles.last_occupied = 0;

	u32 start_time = (u32)SDL_GetTicksNS();
	const Map* map = &gc->wc->maps[gc->wc->current_map];

	for (u16 y = 0; y < gc->wc->maps[gc->wc->current_map].size.y; y++)
	{
		for (u16 x = 0; x < gc->wc->maps[gc->wc->current_map].size.x; x++)
		{
			u8 tile_type = TILE(map, y, x);
			if (tile_type != 0) // Only add walkable tiles
				continue;

			SpawnableTile tile = {
				.tile = { .x = x, .y = y },
				.occupied = false
			};
			DA_APPEND(gc->spawn_tiles, tile);
		}
	}

	_shuffle_tiles(gc);
	u32 end_time = (u32)SDL_GetTicksNS();
	LOG_DEBUG("Map Generated after %d ns", end_time - start_time);

	return true;
}

static inline bool _level_cleared(GameContext* gc)
{
	switch (gc->score)
	{
		default:	
			return true;

		// Forcing change through const
		case 0:
			if (gc->wc->current_map == MAP_TUTORIAL1)	return true;
			((WorldContext*)gc->wc)->current_map = MAP_TUTORIAL1;	break;

		case 3:
			if (gc->wc->current_map == MAP_TUTORIAL2)	return true;
			((WorldContext*)gc->wc)->current_map = MAP_TUTORIAL2;	break;

		case 7:
			if (gc->wc->current_map == MAP_TUTORIAL3)	return true;
			((WorldContext*)gc->wc)->current_map = MAP_TUTORIAL3;	break;
	}

	gc->ghost.count = 0;
	_generate_world(gc);

	return true;
}

bool game_main(GameContext* gc)
{
	if (gc->game_over)
		return true;
	
	if (!_player_move(gc))
		return true;

	_ghost_move(gc);

	if (_is_player_on_hole(gc))
		if (!_score(gc))
			return false;

	if (_ghost_collision(gc))
	{
		gc->player.health++;
		if (gc->player.health == 3)
			gc->game_over = true;
	}

	if (!gc->ghosts_spawned)
		if (!_spawn_ghost(gc))
			return false;

	if (!_level_cleared(gc))
		return false;

	if (!gc->player_spawned)
		if (!_spawn_player(gc))
			ERROR_RETURN("Couldn't find valid player spawn locatation");

	if (!gc->hole_spawned)
		if (!_spawn_hole(gc))
			ERROR_RETURN("Couldn't find valid hole spawn locatation");

	return true;
}

bool game_init(GameContext* gc)
{
	_generate_world(gc);

	gc->ghosts_spawned = true;

	if (!_spawn_player(gc))
		ERROR_RETURN("Couldn't find valid player spawn locatation");

	if (!_spawn_hole(gc))
		ERROR_RETURN("Couldn't find valid hole spawn locatation");

	return true;
}

void game_quit(GameContext* gc)
{
	if (gc->player.pos_list.data)
		SDL_free(gc->player.pos_list.data);
	gc->player.pos_list.data = NULL;

	if (gc->spawn_tiles.data)
		SDL_free(gc->spawn_tiles.data);
	gc->spawn_tiles.data = NULL;
	
	for (int i = 0; i < gc->ghost.count; i++)
	{
		if (gc->ghost.data[i].pos_list.data)
			SDL_free(gc->ghost.data[i].pos_list.data);
		gc->ghost.data[i].pos_list.data = NULL;
	}

	if (gc->ghost.data)
		SDL_free(gc->ghost.data);
	gc->ghost.data = NULL;
}
