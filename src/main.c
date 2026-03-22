// main.c
#define SDL_MAIN_USE_CALLBACKS true
#include <SDL3/SDL_main.h>

#include "main.h"

InputState input = { 0 };

SDL_AppResult SDL_AppIterate(void* appstate)
{
	AppContext* app = (AppContext*)appstate;

	render_main(&app->rc);

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	AppContext* app = (AppContext*)appstate;

	switch (event->type)
	{
		case SDL_EVENT_QUIT:
			return SDL_APP_SUCCESS;

		case SDL_EVENT_KEY_DOWN:
			if (event->key.repeat)
				break;
			switch (event->key.key) 
			{
				case SDLK_W: case SDLK_UP:		input.dir	= DIR_UP;		break;
				case SDLK_S: case SDLK_DOWN:	input.dir	= DIR_DOWN;		break;
				case SDLK_A: case SDLK_LEFT:	input.dir	= DIR_LEFT;		break;
				case SDLK_D: case SDLK_RIGHT:	input.dir	= DIR_RIGHT;	break;
				default: break;
			}
			game_main(&app->gc);
			input.dir = DIR_NONE;
			break;

		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			input.mouse_pos = (f32_2){ .x = event->button.x, .y = event->button.y };
			switch (event->button.button)
			{
				case SDL_BUTTON_LEFT:	input.mouse_left	= true;		break;
				case SDL_BUTTON_RIGHT:	input.mouse_right	= true;		break;
			}
			break;

		case SDL_EVENT_MOUSE_BUTTON_UP:
			input.mouse_pos = (f32_2){ .x = event->button.x, .y = event->button.y };
			switch (event->button.button)
			{
				case SDL_BUTTON_LEFT:	input.mouse_left	= false;	break;
				case SDL_BUTTON_RIGHT:	input.mouse_right	= false;	break;
			}
			break;

		case SDL_EVENT_MOUSE_MOTION:
			input.mouse_pos = (f32_2){ .x = event->motion.x, .y = event->motion.y };
			break;

		case SDL_EVENT_WINDOW_RESIZED:
			input.window_size = (f32_2){ .x = event->window.data1, .y = event->window.data2 };
			break;
	}

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv)
{
	(void)argc;
	(void)argv;

	AppContext* app = (AppContext*)SDL_calloc(1, sizeof(AppContext));
	*appstate = app;

	input.window_size = (f32_2){ 800, 600 };

	if (!app)
		ERROR_EXIT("AppContext memory allocation failed");

	if(!SDL_Init(SDL_INIT_VIDEO))
		ERROR_EXIT("SDL Init Failure");

	if (!SDL_SetAppMetadata("Get In The Hole!!", "v0.2", NULL))
		ERROR_EXIT("Medadata Creation Failure");

	app->window = SDL_CreateWindow("Get In The Hole!!", input.window_size.x, input.window_size.y, 0);
	if (!app->window)
		ERROR_EXIT("Window Creation Failure");

	app->renderer = SDL_CreateRenderer(app->window, NULL);
	if (!app->renderer)
		ERROR_EXIT("Renderer Creation Failure");

	app->rc.gc = &app->gc;
	app->gc.wc = &app->wc;

	if (!render_init(&app->rc, app->renderer))
		ERROR_EXIT("Render Init Failure");

	world_init(&app->wc);

	if (!game_init(&app->gc))
		ERROR_EXIT("Game Init Failure");

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	AppContext* app = (AppContext*)appstate;

	render_quit(&app->rc);
	game_quit(&app->gc);

	if (result == SDL_APP_FAILURE)
		printf("Oopsie, i died! :3\n");
	else
		printf("I somehow didn't die! :D\n");

	SDL_DestroyRenderer(app->renderer);
	SDL_DestroyWindow(app->window);
	SDL_free(app);
}
