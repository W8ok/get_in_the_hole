// main.h
#pragma once

#include <stdio.h>
#include <SDL3/SDL.h>

#include "types.h"
#include "utils.h"
#include "input.h"

#include "game.h"
#include "render.h"
#include "world.h"

typedef struct AppContext {
	RenderContext rc;
	GameContext gc;
	WorldContext wc;

	SDL_Window* window;
	SDL_Renderer* renderer;
} AppContext;
