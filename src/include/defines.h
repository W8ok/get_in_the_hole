// include/defines.h
#pragma once

#include "types.h"

// utils.c
SDL_AppResult Panic(const char* msg);

// game.c
bool game_init(AppState* app);
void main_game(GameContext* gc, Direction dir);
void game_cleanup(GameContext* gc);

// render.c
bool render_init(AppState* app);
void main_render(RenderContext* rc);
void render_cleanup(RenderContext* rc);

// maps.c
extern const Map maps[MAP_COUNT];

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



#define MAX_GHOSTS 255

// Colors
// Base RGB
#define RED       ((SDL_Color){255, 0, 0, 255})
#define GREEN     ((SDL_Color){0, 255, 0, 255})
#define BLUE      ((SDL_Color){0, 0, 255, 255})

// Grayscale
#define WHITE     ((SDL_Color){255, 255, 255, 255})
#define LIGHTGRAY ((SDL_Color){200, 200, 200, 255})
#define GRAY      ((SDL_Color){128, 128, 128, 255})
#define DARKGRAY  ((SDL_Color){30, 30, 30, 255})
#define BLACK     ((SDL_Color){0, 0, 0, 255})
