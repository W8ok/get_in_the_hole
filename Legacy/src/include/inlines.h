// include/inline.h
#pragma once

// Inlines (I learned how to make my code EVEN SILLIER!!!)
static inline uint8_t map_tile_at(const Map* map, int x, int y)
{
  return map->tiles[y * map->w + x];
}

static inline bool map_is_wall(const Map* map, int x, int y)
{
  return map_tile_at(map, x, y) == 1;
}

static inline int tile_index(GameContext* gc, int x, int y)
{
  return y * gc->w + x;
}

static inline bool empty_tile(GameContext* gc, int x, int y)
{
  return map_tile_at(gc->map, x, y) == TILE_EMPTY;
}



