// render.c
#include "include/types.h"
#include "include/defines.h"

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

// Primary Functions
void render_player(RenderContext* rc)
{
  // ADD SPRITES 32x32 should do
  float tile_size = rc->gc->tile_size;

  SDL_FRect player = (SDL_FRect) {
    rc->gc->x * tile_size,
    rc->gc->y * tile_size,
    tile_size,
    tile_size
  };

  _render_fill_rect(rc->renderer, player, RED);
}

void render_ghosts(RenderContext* rc)
{
  float tile_size = rc->gc->tile_size;
  for (int i = 0; i < rc->gc->idx; i++)
  {
    Ghost* g = &rc->gc->ghosts[i];

    // Prevent tick from going out of bounds
    int tick = rc->gc->move_tick;
    if (tick > g->move_count)
      tick = g->move_count;

    SDL_FRect ghost_rect = (SDL_FRect) {
      g->x[tick] * tile_size,
      g->y[tick] * tile_size,
      tile_size,
      tile_size
    };

    _render_fill_rect(rc->renderer, ghost_rect, BLUE);

    // Only render arrow if theres a next move
    if (tick < g->move_count)
    {
      SDL_FPoint ghost_pos = { ghost_rect.x, ghost_rect.y };
      int dx = g->x[tick + 1] - g->x[tick];
      int dy = g->y[tick + 1] - g->y[tick];
      float rotation;

      if (dx == 1)
        rotation =  90; // right
      else if (dx == -1)
        rotation = 270; // left
      else if (dy == 1)
        rotation = 180; // down
      else if (dy == -1)
        rotation = 0;   // up

      _render_sprite(rc->renderer, rc->textures[ARROW], ghost_pos, rotation, 2);
    }
  }
}

void render_hole(RenderContext* rc)
{
  float tile_size = rc->gc->tile_size;

  SDL_FRect hole = (SDL_FRect) {
    rc->gc->hole_x * tile_size,
    rc->gc->hole_y * tile_size,
    tile_size,
    tile_size
  };

  _render_fill_rect(rc->renderer, hole, BLACK);
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
    SDL_RenderLine(rc->renderer, px, 0, px, ph);  // Single line
  }

  for (int y = 0; y <= h; y++) {
    float py = y * tile_size;
    SDL_RenderLine(rc->renderer, 0, py, pw, py);
  }
}

// Main Function
void main_render(RenderContext* rc)
{
  _clear_background(rc->renderer, GRAY);

  render_hole(rc);
  render_player(rc);
  render_ghosts(rc);
  render_grid(rc);


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

  // Texture loading
  const char* texture_paths[TEXTURE_COUNT] = {
    "assets/arrow.png",
  };

  for (int i = 0; i < TEXTURE_COUNT; i++)
  {
    SDL_Surface* surf = SDL_LoadPNG(texture_paths[i]);
    if (!surf) return Panic("Failed to load texture");

    app->rc->textures[i] = SDL_CreateTextureFromSurface(app->rc->renderer, surf);
    SDL_DestroySurface(surf);
    SDL_SetTextureScaleMode(app->rc->textures[i], SDL_SCALEMODE_NEAREST);
  }

  return true;
}

// Cleanup Function
void render_cleanup(RenderContext* rc)
{
  // Texture Cleanup
  for (int i = 0; i < TEXTURE_COUNT; i++)
    if (rc->textures[i]) SDL_DestroyTexture(rc->textures[i]);

  SDL_free(rc);
  rc = NULL;
}
