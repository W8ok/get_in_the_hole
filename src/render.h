// render.h
#pragma once

#include <stdio.h>
#include <math.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "types.h"
#include "utils.h"
#include "game.h"

extern SDL_Renderer* r;

typedef struct RenderContext {
	const GameContext* gc;

	struct {
		SDL_Texture* player;
		SDL_Texture* ghost;
		SDL_Texture* tiles;
		SDL_Texture* hole;
	} tex;

	struct {
		f32 player;
	} animation_state;

	f32_2 offset;
} RenderContext;

void render_main(RenderContext* rc);
bool render_init(RenderContext* rc, SDL_Renderer* renderer);
void render_quit(RenderContext* rc);
