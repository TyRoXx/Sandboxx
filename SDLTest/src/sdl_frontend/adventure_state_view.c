#include "adventure_state_view.h"
#include "camera.h"
#include "avatar_controller.h"
#include "sdl_frontend.h"
#include "base/adventure_state.h"
#include "base/minmax.h"
#include <stdlib.h>
#include <assert.h>


enum
{
	Width = 640, Height = 480, TileWidth = 32
};


typedef struct AdventureStateView
{
	GameStateView base;
	struct AdventureState *state;
	SDLFrontend *front;
	Camera camera;
	AvatarController avatar_controller;
}
AdventureStateView;


static void draw_animation(
	Vector2i pixel_pos,
	SDL_Surface *screen,
	Animation const *animation,
	SDL_Surface *image,
	Direction direction)
{
	AnimationSide const *side = &animation->sides[direction];
	AnimationFrame * const current_frame = side->frames + 0;
	SDL_Rect * const source = &current_frame->section;
	SDL_Rect dest;
	dest.x = (Sint16)pixel_pos.x;
	dest.y = (Sint16)pixel_pos.y;
	SDL_BlitSurface(image, source, screen, &dest);
}

static void draw_appearance(
	Vector2i pixel_pos,
	SDL_Surface *screen,
	AppearanceId appearance_id,
	AppearanceManager const *appearances,
	Direction direction)
{
	Appearance const * const appearance = AppearanceManager_get(
				appearances, appearance_id);
	Animation const *animation;

	if (!appearance)
	{
		return;
	}

	animation = &appearance->layout->animations[Anim_Idle];
	assert(animation);

	draw_animation(pixel_pos, screen, animation, appearance->image, direction);
}

static void draw_entities(
	Camera const *camera,
	SDL_Surface *screen,
	World const *world,
	size_t tile_width,
	AppearanceManager const *appearances)
{
	Mover *begin = (Mover *)Vector_begin(&world->movers);
	Mover * const end = (Mover *)Vector_end(&world->movers);

	assert(world);
	assert(camera);
	assert(screen);
	assert(appearances);

	for (; begin != end; ++begin)
	{
		Vector2i pixel_pos;
		pixel_pos.x = (int)(((float)begin->body.position.x - camera->position.x + get_mover_offset(begin, Dir_East )) * (float)tile_width) + Width  / 2;
		pixel_pos.y = (int)(((float)begin->body.position.y - camera->position.y + get_mover_offset(begin, Dir_South)) * (float)tile_width) + Height / 2;
		draw_appearance(
			pixel_pos,
			screen,
			begin->body.appearance,
			appearances,
			begin->body.direction);
	}
}

static void draw_layered_tile(
	Vector2i pixel_pos,
	SDL_Surface *screen,
	LayeredTile const *tile,
	AppearanceManager const *appearances,
	size_t layer_begin,
	size_t layer_end
)
{
	size_t i;
	for (i = layer_begin; i < layer_end; ++i)
	{
		TileKind const * const layer = tile->layers[i];
		if (layer)
		{
			draw_appearance(pixel_pos,
							screen,
							layer->image_id,
							appearances,
							Dir_North);
		}
	}
}

static void draw_tile_layers(
	Camera const *camera,
	SDL_Surface *screen,
	TileGrid const *tiles,
	size_t tile_width,
	AppearanceManager const *appearances,
	size_t layer_begin,
	size_t layer_end)
{
	int y;

	int visible_begin_idx = (int)(camera->position.x - (float)Width  / (float)tile_width / 2.0f);
	int visible_begin_idy = (int)(camera->position.y - (float)Height / (float)tile_width / 2.0f);
	int visible_end_idx   = (int)(camera->position.x + (float)Width  / (float)tile_width / 2.0f + 1.0f);
	int visible_end_idy   = (int)(camera->position.y + (float)Height / (float)tile_width / 2.0f + 1.0f);

	visible_begin_idx = max_ptrdiff_t(visible_begin_idx, 0);
	visible_begin_idy = max_ptrdiff_t(visible_begin_idy, 0);
	visible_end_idx   = min_ptrdiff_t(visible_end_idx,   (ptrdiff_t)tiles->width);
	visible_end_idy   = min_ptrdiff_t(visible_end_idy,   (ptrdiff_t)tiles->height);

	for (y = visible_begin_idy; y < visible_end_idy; ++y)
	{
		int x;
		for (x = visible_begin_idx; x < visible_end_idx; ++x)
		{
			LayeredTile const * const tile = TileGrid_get(tiles, (size_t)x, (size_t)y);
			Vector2i pixel_pos;

			assert(tile);

			pixel_pos.x = (int)((float)tile_width * ((float)x - camera->position.x) + Width  / 2.0f);
			pixel_pos.y = (int)((float)tile_width * ((float)y - camera->position.y) + Height / 2.0f);

			draw_layered_tile(
				pixel_pos,
				screen,
				tile,
				appearances,
				layer_begin,
				layer_end
				);
		}
	}
}


static GameStateView *AdventureStateView_create(GameState *state, struct SDLFrontend *front)
{
	AdventureState * const adv_state = (AdventureState *)state;
	AdventureStateView * const adv_view = malloc(sizeof(*adv_view));

	assert(adv_state);
	assert(front);

	if (!adv_view)
	{
		goto fail_0;
	}

	if (!Camera_init(&adv_view->camera))
	{
		goto fail_1;
	}

	if (!AvatarController_init(&adv_view->avatar_controller, adv_state->avatar))
	{
		goto fail_2;
	}

	adv_view->front = front;
	adv_view->state = adv_state;
	return (GameStateView *)adv_view;

fail_2:
	Camera_free(&adv_view->camera);

fail_1:
	free(adv_view);

fail_0:
	return 0;
}

static void AdventureStateView_destroy(GameStateView *view)
{
	AdventureStateView * const adv_view = (AdventureStateView *)view;

	AvatarController_free(&adv_view->avatar_controller);
	Camera_free(&adv_view->camera);
	free(view);
}

static void AdventureStateView_update(GameStateView *view)
{
	AdventureStateView * const adv_view = (AdventureStateView *)view;

	AvatarController_update(&adv_view->avatar_controller);
}

static void AdventureStateView_draw(GameStateView *view)
{
	AdventureStateView * const adv_view = (AdventureStateView *)view;
	SDL_Surface * const screen = adv_view->front->screen;

	if (adv_view->state->avatar)
	{
		Camera_focus_on(&adv_view->camera, adv_view->state->avatar);
	}

	assert(TILE_LAYER_COUNT == 3);

	/*draw layers 0 and 1*/
	draw_tile_layers(
		&adv_view->camera,
		screen,
		&adv_view->state->world.tiles,
		TileWidth,
		&adv_view->front->data.appearances,
		0,
		2
		);

	draw_entities(
		&adv_view->camera,
		screen,
		&adv_view->state->world,
		TileWidth,
		&adv_view->front->data.appearances
		);

	/*draw layer 2*/
	draw_tile_layers(
		&adv_view->camera,
		screen,
		&adv_view->state->world.tiles,
		TileWidth,
		&adv_view->front->data.appearances,
		2,
		3
		);
}

static void AdventureStateView_handle_event(GameStateView *view, SDL_Event const *event)
{
	AdventureStateView * const adv_view = (AdventureStateView *)view;
	AvatarController * const avatar_controller = &adv_view->avatar_controller;

	if (event->type == SDL_KEYUP)
	{
		switch (event->key.keysym.sym)
		{
		case SDLK_w: AvatarController_handle_input(avatar_controller, Dir_North, 0); break;
		case SDLK_a: AvatarController_handle_input(avatar_controller, Dir_West, 0); break;
		case SDLK_s: AvatarController_handle_input(avatar_controller, Dir_South, 0); break;
		case SDLK_d: AvatarController_handle_input(avatar_controller, Dir_East, 0); break;
		default:
			break;
		}
	}
	else if (event->type == SDL_KEYDOWN)
	{
		switch (event->key.keysym.sym)
		{
		case SDLK_w: AvatarController_handle_input(avatar_controller, Dir_North, 1); break;
		case SDLK_a: AvatarController_handle_input(avatar_controller, Dir_West, 1); break;
		case SDLK_s: AvatarController_handle_input(avatar_controller, Dir_South, 1); break;
		case SDLK_d: AvatarController_handle_input(avatar_controller, Dir_East, 1); break;
		default:
			break;
		}
	}
}

GameStateViewType const AdventureStateViewType =
{
	AdventureStateView_create,
	AdventureStateView_destroy,
	AdventureStateView_update,
	AdventureStateView_draw,
	AdventureStateView_handle_event,
};
