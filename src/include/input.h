// input.h
#pragma once

#include <SDL3/SDL.h>

#include "types.h"

typedef enum {
	DIR_NONE,
	DIR_UP,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT,
} Direction;

typedef struct InputState {
	Direction dir;
	
	f32_2 mouse_pos;
	bool mouse_left, mouse_right;

	f32_2 window_size;
} InputState;

extern InputState input;
