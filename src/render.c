// render.c
#include "render.h"

SDL_Renderer* r = NULL;

static inline bool _set_color(SDL_Color color)
{
	return SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
}

static inline bool _render_texture(SDL_Texture* texture, const f32_4* src, const f32_4* dst)
{
	return SDL_RenderTexture(r, texture, (const SDL_FRect*)src, (const SDL_FRect*)dst);
}

static inline void _render_player(RenderContext* rc)
{
	// I love pointers
	const f32 tile_size = rc->gc->wc->tile_size;

	f32 animation_count;
	SDL_GetTextureSize(rc->tex.player, &animation_count, NULL);
	animation_count /= tile_size;

	static u32 last_tick = 0;
	u32 now = SDL_GetTicks();

	if ((now - last_tick) >= 1000.0f)
	{
		last_tick = now;
		rc->animation_state.player++;
		rc->animation_state.player = fmod(rc->animation_state.player, animation_count);
	}


	f32_4 src = {
		.x = rc->animation_state.player * tile_size,
		.y = rc->gc->player.health * tile_size,
		.w = tile_size,
		.h = tile_size
	};

	f32_4 dst = {
		.x = rc->gc->player.current_pos.x * tile_size + rc->offset.x,
		.y = rc->gc->player.current_pos.y * tile_size + rc->offset.y,
		.w = tile_size,
		.h = tile_size
	};

	if (!_render_texture(rc->tex.player, &src, &dst))
		LOG_WARN("Player Rendering ERROR");
}

static inline void _render_hole(RenderContext* rc)
{
	f32 tile_size = rc->gc->wc->tile_size;

	f32_4 src = {
		.x = 0,
		.y = 0,
		.w = tile_size,
		.h = tile_size
	};

	f32_4 dst = {
		.x = rc->gc->hole_pos.x * tile_size + rc->offset.x,
		.y = rc->gc->hole_pos.y * tile_size + rc->offset.y,
		.w = tile_size,
		.h = tile_size
	};

	if (!_render_texture(rc->tex.hole, &src, &dst))
		LOG_WARN("Hole Rendering ERROR");
}

static inline void _render_ghosts(RenderContext* rc)
{
	if (rc->gc->game_over)
		return;

	if (!rc->gc->ghosts_spawned)
		return;

	f32 tile_size = rc->gc->wc->tile_size;

	f32_4 src = {
		.x = 0,
		.y = 0,
		.w = tile_size,
		.h = tile_size
	};

	for (u16 i = 0; i < rc->gc->ghost.count; i++)
	{
		if (!rc->gc->ghost.data[i].active)
			continue;

		f32_4 dst = {
			.x = rc->gc->ghost.data[i].current_pos.x * tile_size + rc->offset.x,
			.y = rc->gc->ghost.data[i].current_pos.y * tile_size + rc->offset.y,
			.w = tile_size,
			.h = tile_size
		};

		if (!_render_texture(rc->tex.ghost, &src, &dst))
			LOG_WARN("Ghost%d Rendering ERROR", i);
	}
}

static inline void _render_map(RenderContext* rc)
{
	const WorldContext* wc = rc->gc->wc;
	f32 tile_size = wc->tile_size;

	for (u8 y = 0; y < wc->maps[wc->current_map].size.y; y++)
	{
		for (u8 x = 0; x < wc->maps[wc->current_map].size.x; x++)
		{
			u8 tile_type = TILE(&wc->maps[wc->current_map], y, x);
			if (!tile_type)
				continue;

			f32_4 src = {
				.x = (tile_type-1) * tile_size,
				.y = 0,
				.w = tile_size,
				.h = tile_size
			};

			f32_4 dst = {
				.x = tile_size * x + rc->offset.x,
				.y = tile_size * y + rc->offset.y,
				.w = tile_size,
				.h = tile_size
			};

			if (!_render_texture(rc->tex.tiles, &src, &dst))
				LOG_WARN("Tile (%d, %d) Rendering ERROR", x, y);
		}
	}
}

static inline bool _clear_background(SDL_Color color)
{
	if (!_set_color(color))
		return false;
	return SDL_RenderClear(r);
}

static inline void _get_offset(RenderContext* rc)
{
	const WorldContext* wc = rc->gc->wc;
	f32_2 map_size = (f32_2){
		.x = wc->maps[wc->current_map].size.x * wc->tile_size,
		.y = wc->maps[wc->current_map].size.y * wc->tile_size
	};

	rc->offset = (f32_2){
		.x = (input.window_size.x - map_size.x) / 2,
		.y = (input.window_size.y - map_size.y) / 2,
	};
}

void render_debug_lines()
{
	_set_color(RED);
	SDL_RenderLine(r, 0, 0, input.window_size.x, input.window_size.y);
	SDL_RenderLine(r, input.window_size.x, 0, 0, input.window_size.y);
	
	_set_color(BLUE);
	SDL_RenderLine(r, input.window_size.x/2, 0, input.window_size.x/2, input.window_size.y);
	SDL_RenderLine(r, 0, input.window_size.y/2, input.window_size.x, input.window_size.y/2);
}

void render_main(RenderContext* rc)
{
	if (!_clear_background(DARKGRAY))
		LOG_WARN("Clear Background ERROR");

	render_debug_lines();

	_get_offset(rc);

	_render_player(rc);
	_render_hole(rc);
	_render_ghosts(rc);
	_render_map(rc);

	SDL_RenderPresent(r);
}

static inline SDL_Texture* _load_texture(const char* texture_path)
{
	char full_path[256];
	snprintf(full_path, sizeof(full_path), "./assets/%s", texture_path);
	SDL_Texture* texture = IMG_LoadTexture(r, full_path);
	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
	return texture;
}

bool render_init(RenderContext* rc, SDL_Renderer* renderer)
{
	r = renderer;

	if (!SDL_SetRenderVSync(r, -1))			// Adaptive
		if (!SDL_SetRenderVSync(r, 1))		// Regular
			if (!SDL_SetRenderVSync(r, 0))	// No Vsync
				ERROR_RETURN("VSync Initialization Failed");

	// TODO
	// Add a loading bar

	i32 current_texture = 0;
	i32 total_textures = sizeof(rc->tex) / sizeof(SDL_Texture*);

	// I want no questions...
	// Yes I know an inline function would be better
	// No I don't care
#define LOAD_TEXTURE(member, name) \
	do { \
		current_texture++; \
		i32 percent = (current_texture * 100) / total_textures; \
		rc->tex.member = _load_texture(name); \
		if (!rc->tex.member) \
			ERROR_RETURN("Failed to load texture: %s (%d%%)", name, percent); \
		else \
			LOG_INFO("Loaded texture: %s (%d%%)", name, percent); \
	} while (0)

	LOAD_TEXTURE(player,	"player.png");
	LOAD_TEXTURE(ghost,		"ghost.png");
	LOAD_TEXTURE(tiles,		"tiles.png");
	LOAD_TEXTURE(hole,		"hole.png");

#undef LOAD_TEXTURE

	SDL_SetTextureAlphaMod(rc->tex.ghost, 196);

	if (!SDL_RenderPresent(r))
		ERROR_RETURN("Initial Render Failure");

	return true;
}

void render_quit(RenderContext* rc)
{
	SDL_DestroyTexture(rc->tex.player);
	SDL_DestroyTexture(rc->tex.ghost);
	SDL_DestroyTexture(rc->tex.tiles);
	SDL_DestroyTexture(rc->tex.hole);
}
