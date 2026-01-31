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
