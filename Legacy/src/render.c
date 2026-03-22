// render.c
#include "include/types.h"
#include "include/defines.h"
#include "include/inlines.h"

// Helper Fucntions
void _clear_background(SDL_Renderer* renderer, SDL_Color color)
{
  // Draw Background
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_RenderClear(renderer);
}

void _render_rect(SDL_Renderer* renderer, SDL_FRect rect, SDL_Color color)
{
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_RenderRect(renderer, &rect);
}

void _render_fill_rect(SDL_Renderer* renderer, SDL_FRect rect, SDL_Color color)
{
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_RenderFillRect(renderer, &rect);
}

void _render_sprite(SDL_Renderer* renderer, SDL_Texture* texture, SDL_FPoint pos, float rotation, float size)
{
  float tex_w, tex_h;
  SDL_GetTextureSize(texture, &tex_w, &tex_h);

  SDL_FRect sprite = (SDL_FRect) {
    pos.x,
    pos.y,
    tex_w * size,
    tex_h * size
  };
  
  SDL_RenderTextureRotated(renderer, texture, NULL, &sprite, rotation, NULL, SDL_FLIP_NONE);
}

static float _approach(float current, float target, float delta_time)
{
  const float speed = 0.005f; // tiles per ms
  float delta = target - current;
  float step = speed * delta_time;

  if (fabsf(delta) <= step)
    return target; // reached target
  return current + (delta > 0 ? step : -step);
}

static SDL_FPoint _get_map_offset(const GameContext* gc)
{
    float screen_w = 800;
    float screen_h = 600;

    float map_pixel_w = gc->w * gc->tile_size;
    float map_pixel_h = gc->h * gc->tile_size;

    SDL_FPoint offset = {
        .x = (screen_w - map_pixel_w) / 2,
        .y = (screen_h - map_pixel_h) / 2
    };

    return offset;
}

// Primary Functions
void render_player(RenderContext* rc, float delta_time)
{
  float tile_size = rc->gc->tile_size;

  SDL_FPoint offset = _get_map_offset(rc->gc);

  // Use interpolated render positions
  rc->player_render.render_x = _approach(rc->player_render.render_x, rc->gc->x, delta_time);
  rc->player_render.render_y = _approach(rc->player_render.render_y, rc->gc->y, delta_time);

  // Update animation timer
  rc->frame_timer += delta_time;

  if (!rc->gc->game_over)
  {
    if (rc->frame_timer >= 1000)
    {
      rc->idle_frame = (rc->idle_frame + 1) % 4; // 4 frames in a row
      rc->frame_timer = 0;
    }
  }
  else
  {
    if (rc->idle_frame < 3 && rc->frame_timer >= 1000)
    {
      rc->idle_frame++;
      rc->frame_timer = 0;
    }

  }

  SDL_FRect src = {
    .x = rc->idle_frame * 32.0f,
    .y = rc->gc->hp * 32.0f,
    .w = 32.0f,
    .h = 32.0f
  };

  SDL_FRect player = {
    .x = rc->player_render.render_x * tile_size + offset.x,
    .y = rc->player_render.render_y * tile_size + offset.y,
    .w = tile_size,
    .h = tile_size
  };

  SDL_RenderTexture(rc->renderer, rc->textures[PLAYER], &src, &player);
}

void render_ghosts(RenderContext* rc, float delta_time)
{
  if (rc->gc->game_over)
    return;

  float tile_size = rc->gc->tile_size;
  SDL_FPoint offset = _get_map_offset(rc->gc);

  for (int i = 0; i < rc->gc->idx; i++)
  {
    Ghost* g = &rc->gc->ghosts[i];
    RenderPos* rpos = &rc->ghost_render[i];

    int tick = rc->gc->move_tick;
    if (tick > g->move_count)
      tick = g->move_count;

    float target_x = g->x[tick];
    float target_y = g->y[tick];

    // Interpolate render position
    rpos->render_x = _approach(rpos->render_x, target_x, delta_time);
    rpos->render_y = _approach(rpos->render_y, target_y, delta_time);

    Direction dir = DIR_UP;
    if (tick <= g->move_count)
    {
      int dx = g->x[tick + 1] - g->x[tick];
      int dy = g->y[tick + 1] - g->y[tick];

      if (dx == 1)
        dir = DIR_RIGHT;
      else if (dx == -1)
        dir = DIR_LEFT;
      else if (dy == 1)
        dir = DIR_DOWN;
      else if (dy == -1)
        dir = DIR_UP;
      else
        dir = DIR_NONE;
    }

    SDL_FRect src = (SDL_FRect) {
      dir * 32.0f,
      0.0f,
      32.0f,
      32.0f
    };

    SDL_FRect ghost_rect = (SDL_FRect) {
      rpos->render_x * tile_size + offset.x,
      rpos->render_y * tile_size + offset.y,
      tile_size,
      tile_size
    };

    SDL_RenderTexture(rc->renderer, rc->textures[GHOST], &src, &ghost_rect);
  }
}

void render_hole(RenderContext* rc)
{
  float tile_size = rc->gc->tile_size;
  SDL_FPoint offset = _get_map_offset(rc->gc);

  SDL_FRect hole = (SDL_FRect) {
    rc->gc->hole_x * tile_size + offset.x,
    rc->gc->hole_y * tile_size + offset.y,
    tile_size,
    tile_size
  };

  SDL_RenderTexture(rc->renderer, rc->textures[HOLE], NULL, &hole);
}

void render_grid(const RenderContext* rc)
{
  SDL_Color grid_color = (SDL_Color) {50, 50, 50, 255};
  SDL_SetRenderDrawColor(rc->renderer, grid_color.r, grid_color.g, grid_color.b, grid_color.a);

  int w = rc->gc->w;
  int h = rc->gc->h;
  int tile_size = rc->gc->tile_size;

  // Prepare line points
  int pw = w * tile_size;
  int ph = h * tile_size;

  for (int x = 0; x <= w; x++) {
    float px = x * tile_size;
    SDL_RenderLine(rc->renderer, px, 0, px, ph);
  }

  for (int y = 0; y <= h; y++) {
    float py = y * tile_size;
    SDL_RenderLine(rc->renderer, 0, py, pw, py);
  }
}

void render_map(RenderContext* rc)
{
  const Map* map = rc->gc->map;
  float tile = rc->gc->tile_size;
  SDL_FPoint offset = _get_map_offset(rc->gc);

  for (int y = 0; y < map->h; y++)
  {
    for (int x = 0; x < map->w; x++)
    {
      uint8_t t = map_tile_at(map, x, y);

      if (t == TILE_WALL)
      {
        SDL_FRect wall = {
          x * tile + offset.x,
          y * tile + offset.y,
          tile,
          tile
        };

        SDL_RenderTexture(rc->renderer, rc->textures[TILES], NULL, &wall);
      }
    }
  }
}

float get_delta_time()
{
  static uint32_t last_time = 0;
  uint32_t current_time = SDL_GetTicks();
  float delta_time = (float)(current_time - last_time);
  last_time = current_time;
  return delta_time;
}

// Main Function
void main_render(RenderContext* rc)
{
  float delta_time = get_delta_time();
  _clear_background(rc->renderer, DARKGRAY);

  //render_grid(rc);
  render_map(rc);
  render_hole(rc);
  render_player(rc, delta_time);
  render_ghosts(rc, delta_time);

  // Render what is drawn
  SDL_RenderPresent(rc->renderer);
}

// Initialization
bool render_init(AppState* app)
{
  app->rc = SDL_malloc(sizeof(RenderContext));
  if (!app->rc || !app->renderer)
      return false;
  
  app->rc->renderer = app->renderer;
  app->rc->gc = app->gc;

  // Initialize interpolated render positions
  app->rc->player_render.render_x = app->gc->x;
  app->rc->player_render.render_y = app->gc->y;

  app->rc->ghost_render = SDL_calloc(MAX_GHOSTS, sizeof(RenderPos));

  // Texture loading
  const char* texture_paths[TEXTURE_COUNT] = {
    "assets/arrow.bmp",
    "assets/player.bmp",
    "assets/ghost.bmp",
    "assets/hole.bmp",
    "assets/tiles.bmp",
  };

  for (int i = 0; i < TEXTURE_COUNT; i++)
  {
    SDL_Surface* surf = SDL_LoadBMP(texture_paths[i]);
    if (!surf)
      return Panic("Failed to load texture");

    app->rc->textures[i] = SDL_CreateTextureFromSurface(app->rc->renderer, surf);
    SDL_DestroySurface(surf);
    SDL_SetTextureScaleMode(app->rc->textures[i], SDL_SCALEMODE_NEAREST);
  }

  return true;
}

// Cleanup Function
void render_cleanup(RenderContext* rc)
{
  if (!rc) return;

  // Texture Cleanup
  for (int i = 0; i < TEXTURE_COUNT; i++)
    if (rc->textures[i])
      SDL_DestroyTexture(rc->textures[i]);

  // Free ghost_render safely
  if (rc->ghost_render)
  {
    SDL_free(rc->ghost_render);
    rc->ghost_render = NULL;
  }

  // Free the RenderContext itself
  SDL_free(rc);
}

