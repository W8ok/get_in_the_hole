// main.c
#define SDL_MAIN_USE_CALLBACKS 1  // Use the callbacks instead of main()
#include <SDL3/SDL_main.h>

#include "include/types.h"
#include "include/defines.h"

SDL_AppResult SDL_AppIterate(void *appstate)
{
  AppState *app = (AppState *)appstate;

  main_render(app->rc);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
  AppState *app = (AppState *)appstate;

  if (event->type == SDL_EVENT_QUIT)
    return SDL_APP_SUCCESS;

  if (event->type == SDL_EVENT_KEY_DOWN)
  {
    switch (event->key.key) 
    {
      case SDLK_W: case SDLK_UP: main_game(app->gc, DIR_UP); break;
      case SDLK_S: case SDLK_DOWN: main_game(app->gc, DIR_DOWN); break;
      case SDLK_A: case SDLK_LEFT: main_game(app->gc, DIR_LEFT); break;
      case SDLK_D: case SDLK_RIGHT: main_game(app->gc, DIR_RIGHT); break;
    }
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
  (void)argc;
  (void)argv;
  
  AppState *app = (AppState *)SDL_calloc(1, sizeof(AppState));
  *appstate = app;

  if (!app)
    return Panic("AppState memory allocation failed");

  // SDL stuff
  if(!SDL_Init(SDL_INIT_VIDEO))
    return Panic("SDL_Init failed");

  if (!SDL_SetAppMetadata("Get In The Hole!!", "v0.1", NULL)) // Not strictly needed but nice to have
    return Panic("Medadata creation failed");

  // Create window and renderer at 800x600
  if (!SDL_CreateWindowAndRenderer("Get In The Hole!!", 800, 600, SDL_WINDOW_RESIZABLE, &app->window, &app->renderer))
    return Panic("Window/Renderer creation failed");

  if (!SDL_SetRenderLogicalPresentation(app->renderer, 800, 600, SDL_LOGICAL_PRESENTATION_LETTERBOX))
    return Panic("Logical Presentation creation failed");

  // My own stuff (mmmm pointers :3c)
  if (!game_init(app))
    return Panic("game_init failed");

  if (!render_init(app))
    return Panic("render_init failed");

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
  AppState *app = (AppState *)appstate;
  render_cleanup(app->rc);
  game_cleanup(app->gc);

  if (result == SDL_APP_FAILURE)
    printf("Oopsie, i died! :3\n");
  else
    printf("I somehow didn't die! :D\n");

  SDL_DestroyRenderer(app->renderer);
  SDL_DestroyWindow(app->window);
  SDL_free(app);
}
