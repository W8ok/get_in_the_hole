// types.h
#pragma once

// Libraries
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <SDL3/SDL.h>

// Enums
typedef enum {
  DIR_UP,
  DIR_DOWN,
  DIR_LEFT,
  DIR_RIGHT
} Direction;

typedef enum {
  ARROW,
  TEXTURE_COUNT
} TextureID;

typedef enum {
  DEFAULT,  // 5x5
  MAP_COUNT
} MapID;

typedef enum {
  TILE_EMPTY,
  TILE_WALL,
} TileType;


// Common Types
typedef struct {
  uint8_t w, h;
  const uint8_t *tiles;
} Map;

typedef struct {
  uint8_t *x, *y; // Both are arrays
  int move_count;
  bool in_hole;
} Ghost;

// Context Types
typedef struct {
  // Player
  int x, y;
  Direction* move_history;
  int move_tick;
  bool spawned;
  Ghost* ghosts;
  int idx;

  // Tiles
  int tile_size;
  int w, h;
  int hole_x, hole_y;
  const Map* map;
  bool* player_spawned_here;
  bool* hole_spawned_here;
} GameContext;

typedef struct {
  SDL_Renderer* renderer;

  const GameContext* gc;

  // Textures
  SDL_Texture* textures[TEXTURE_COUNT];
  SDL_Texture* maps[MAP_COUNT];
} RenderContext;

typedef struct {
  // Normal stuff
  SDL_Window* window;
  SDL_Renderer* renderer;

  // Types

  // Contexts
  GameContext* gc;
  RenderContext* rc;
} AppState;
