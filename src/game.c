// game.c
#include "include/types.h"
#include "include/defines.h"

// Helper Functions
int _get_random_int(int min, int max)
{
  printf("_get_random_int\n");
  return min + rand() % (max - min + 1);  
}

bool _valid_move(GameContext* gc, Direction dir)
{
  int new_x = gc->x;
  int new_y = gc->y;

  switch (dir)
  {
    case DIR_UP:    new_y--; break;
    case DIR_DOWN:  new_y++; break;
    case DIR_LEFT:  new_x--; break;
    case DIR_RIGHT: new_x++; break;
    default: return false;
  }

    // Check map bounds
  if (new_x < 0 || new_x >= gc->w || new_y < 0 || new_y >= gc->h)
    return false;

    // Wall collision
  else if (map_is_wall(gc->map, new_x, new_y))
    return false;

  else
    return true;
}

bool _player_on_hole(GameContext* gc)
{
  printf("_player_on_hole\n");
  if (gc->x == gc->hole_x && gc->y == gc->hole_y)
    return true;

  else
    return false;
}

// Primary Functions
void player_move(GameContext* gc, Direction dir)
{
  printf("player_move\n");
  Ghost* g = &gc->ghosts[gc->idx];

  uint8_t *new_x = realloc(g->x, (gc->move_tick + 1) * sizeof(uint8_t));
  uint8_t *new_y = realloc(g->y, (gc->move_tick + 1) * sizeof(uint8_t));

  if (!new_x || !new_y)
  {
    free(new_x);
    free(new_y);
    return;
  }
    
  g->x = new_x;
  g->y = new_y;

  g->x[gc->move_tick] = gc->x;
  g->y[gc->move_tick] = gc->y;

  gc->move_tick++;

  switch (dir)
  {
    case DIR_UP: 
      if (gc->y == 0)
        break;

      gc->y--;
      break;

    case DIR_DOWN:
      if (gc->y == gc->h - 1)
        break;

      gc->y++;
      break;

    case DIR_LEFT:
      if (gc->x == 0)
        break;

      gc->x--;
      break;

    case DIR_RIGHT:
      if (gc->x == gc->w - 1)
        break;

      gc->x++;
      break;
  }
}

void random_spawn(GameContext* gc)
{
  printf("random_spawn\n");
  
  // Spawn player in an empty tile
  // and not on a previous player spawn location
  do {
    gc->x = _get_random_int(0, gc->w - 1);
    gc->y = _get_random_int(0, gc->h - 1);
  } while (!empty_tile(gc, gc->x, gc->y)
      || gc->player_spawned_here[tile_index(gc, gc->x, gc->y)]);

  gc->player_spawned_here[tile_index(gc, gc->x, gc->y)] = true;

  // Makes sure the hole doesnt spawn on the player
  // Expanded to also make sure it spawns on an empty tile
  do {
    gc->hole_x = _get_random_int(0, gc->w - 1);
    gc->hole_y = _get_random_int(0, gc->h - 1);
  } while (!empty_tile(gc, gc->hole_x, gc->hole_y)
      || gc->player_spawned_here[tile_index(gc, gc->hole_x, gc->hole_y)]
      || gc->hole_spawned_here[tile_index(gc, gc->hole_x, gc->hole_y)]
      || (gc->hole_x == gc->x && gc->hole_y == gc->y));

  gc->hole_spawned_here[tile_index(gc, gc->hole_x, gc->hole_y)] = true;
  gc->spawned = true;
}

void score(GameContext* gc)
{
  printf("score\n\n"); // 2 extra lines when scoring
  Ghost* g = &gc->ghosts[gc->idx];

  uint8_t *new_x = realloc(g->x, (gc->move_tick + 1) * sizeof(uint8_t));
  uint8_t *new_y = realloc(g->y, (gc->move_tick + 1) * sizeof(uint8_t));

  if (!new_x || !new_y)
  {
    free(new_x);
    free(new_y);
    return;
  }
    
  g->x = new_x;
  g->y = new_y;

  g->x[gc->move_tick] = gc->x;
  g->y[gc->move_tick] = gc->y;

  g->move_count = gc->move_tick;

  gc->move_tick = 0;
  gc->idx++;
  gc->spawned = false;

  for (int i = 0; i < gc->idx; i++)
    gc->ghosts[i].in_hole = false;
}

void ghost_collision(GameContext* gc)
{
  printf("ghost_collision\n");
  for (int i = 0; i < gc->idx; i++)
  {
    Ghost* g = &gc->ghosts[i];
    if (g->in_hole || gc->move_tick > g->move_count)
      continue;

    if (gc->move_tick >= g->move_count+1)
      g->in_hole = true;
  }
}

// Main Function
void main_game(GameContext* gc, Direction dir)
{
  printf("\n");
  
  if (!_valid_move(gc, dir))
    return;

  player_move(gc, dir);

  ghost_collision(gc);

  if (_player_on_hole(gc))
    score(gc);

  if (!gc->spawned)
    random_spawn(gc);
}

// Initialization
bool game_init(AppState* app)
{
  printf("game_init\n");
  app->gc = calloc(1, sizeof(GameContext));
  if (!app->gc)
    return false;

  srand(time(NULL));

  app->gc->tile_size = 64;
  app->gc->w = 10;
  app->gc->h = 10;

  app->gc->ghosts = calloc(MAX_GHOSTS, sizeof(Ghost));
  if (!app->gc->ghosts)
    return false;
  
  app->gc->ghosts[app->gc->idx].x = NULL;
  app->gc->ghosts[app->gc->idx].y = NULL;
  app->gc->ghosts[app->gc->idx].move_count = 0;

  app->gc->map = &maps[DEFAULT];
  app->gc->w = app->gc->map->w;
  app->gc->h = app->gc->map->h;

  int tile_count = app->gc->w * app->gc->h;
  app->gc->player_spawned_here = calloc(tile_count, sizeof(bool));
  app->gc->hole_spawned_here = calloc(tile_count, sizeof(bool));

  if (!app->gc->player_spawned_here || !app->gc->hole_spawned_here)
    return false;

  random_spawn(app->gc);

  return true;
}

// Cleanup Function
void game_cleanup(GameContext* gc)
{
  printf("game_cleanup\n");
  for (int i = 0; i < gc->idx; i++) {
    free(gc->ghosts[i].x);  // Free x array
    free(gc->ghosts[i].y);  // Free y array
  }
  free(gc->ghosts);
  gc->ghosts = NULL;
  
  free(gc->player_spawned_here);
  free(gc->hole_spawned_here);
  gc->player_spawned_here = NULL;
  gc->hole_spawned_here = NULL;

  free(gc);
  gc = NULL;
}

